//
// Created by 26372 on 2022/4/17.
//

#include "webserver.h"
#include <utility>


Webserver::Webserver(Webserver::port_t port, bool is_et) : _lfd(-1), _port(port), _tp(nullptr) {
	_lfd_event = EPOLLRDHUP | EPOLLIN;
	if (is_et) {
		_lfd_event |= EPOLLET;
	}
	init_server();
}


Webserver::~Webserver() {
	if (_lfd >= 0) {
		close(_lfd);
	}
}


void Webserver::init_server() {
	_lfd = socket(AF_INET, SOCK_STREAM, 0);
	if (_lfd < 0) {
		LOG_FATAL("socket create error!");
		exit(1);
	}
	// 设置为非阻塞套接字
	int flags = fcntl(_lfd, F_GETFL, 0);
	fcntl(_lfd, F_SETFL, flags | O_NONBLOCK);
	// 设置端口复用
	int opt = 1;
	setsockopt(_lfd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

	sockaddr_in local = {};
	memset(&local, 0, sizeof(local));
	local.sin_family = AF_INET;
	local.sin_port = htons(_port);
	local.sin_addr.s_addr = INADDR_ANY;

	if (bind(_lfd, (sockaddr *) &local, sizeof(local)) == -1) {
		LOG_FATAL("bind error!");
		exit(1);
	}

	if (listen(_lfd, 128) == -1) {
		LOG_FATAL("listen error!");
		exit(1);
	}

	// 初始化线程池
	try {
		_tp = new thread_pool;
	} catch (...) {
		LOG_FATAL("thread pool create error!");
		exit(3);
	}
	_tp->start();
	// 忽略掉SIGPIPE信号
	signal(SIGPIPE, SIG_IGN);
	LOG_INFO("server start on 8080 port");
}


void Webserver::get(const std::string &url, Webserver::request_trigger trigger) {
	_events_map["GET:" + url] = std::move(trigger);
}


void Webserver::post(const std::string &url, Webserver::request_trigger trigger) {
	_events_map["POST:" + url] = std::move(trigger);
}


void Webserver::static_path(std::string path) {
	HttpConnection::_static_resource_root_path = std::move(path);
}


[[noreturn]] void Webserver::dispatch() {
	_ioc.add_event(_lfd, _lfd_event);

	while (true) {
		int nready = _ioc.wait(-1);
		auto *ready = _ioc.get_events();
		for (int i = 0; i < nready; ++i) {
			epoll_event ev = ready[i];
			// 接收新连接
			if (ev.data.fd == _lfd) {
				accept_cb();
				continue;
			}

			// 处理出错
			if (ev.events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) {
				LOG_ERROR("epollhup error fd = %d", ev.data.fd);
				error_cb(this, ev.data.fd);
				continue;
			}

			// 处理读取
			if (ev.events & EPOLLIN) {
				_tp->push_task(recv_cb, this, static_cast<int>(ev.data.fd));
			}

			// 处理写入
			if (ev.events & EPOLLOUT) {
				_tp->push_task(write_cb, this, static_cast<int>(ev.data.fd));
			}
		}
	}
}


void Webserver::accept_cb() {
	do {
		sockaddr_in origin_addr = {};
		socklen_t origin_addr_len = sizeof(origin_addr);
		memset(&origin_addr, 0, sizeof(origin_addr));
		int cfd = accept(_lfd, (sockaddr *) &origin_addr, &origin_addr_len);
		if (cfd <= 0) {
			LOG_WARRING("accept_cb client failed!");
			return;
		}
		LOG_INFO("accept a new client fd = %d", cfd);

		_ioc.add_event(cfd, EPOLLIN | EPOLLET | EPOLLRDHUP);
		_connects[cfd] = HttpConnection(cfd, origin_addr);
	} while (_lfd_event & EPOLLET);
}


void Webserver::recv_cb(Webserver *ws, int fd) {
	LOG_INFO("recv from fd = %d client message", fd);
	// 读取数据
	ssize_t nread = ws->_connects[fd].receive();

	if (nread <= 0) {
		LOG_ERROR("receive error fd = %d", fd);
		error_cb(ws, fd);
	} else {
		if (ws->_connects[fd].is_close()) {
			error_cb(ws, fd);
		} else {
			if (!ws->_ioc.modify(fd, EPOLLOUT | EPOLLET | EPOLLRDHUP)) {
				error_cb(ws, fd);
			}
		}
	}
}


void Webserver::write_cb(Webserver *ws, int fd) {
	// 连接未关闭，响应报文准备完毕
	if (!ws->_connects[fd].is_close() && ws->_connects[fd].ready()) {
		LOG_INFO("write call fd = %d, thread id = %ld", fd, pthread_self());
		std::string url = ws->_connects[fd].get_method() + ":" + ws->_connects[fd].get_path();
		if (ws->_events_map.find(url) != ws->_events_map.end()) {
			(ws->_events_map[url])(ws->_connects[fd]._request, ws->_connects[fd]._response);
			ws->_connects[fd].send();
		} else {
			ws->_connects[fd].send_file();
		}

		if (ws->_connects[fd].keep_alive()) {
			if (!ws->_ioc.modify(fd, EPOLLIN | EPOLLET | EPOLLRDHUP)) {
				error_cb(ws, fd);
			}
		} else {
			error_cb(ws, fd);
		}
	}
}


void Webserver::error_cb(Webserver *ws, int fd) {
	// 关闭连接
	LOG_INFO("server name = %s, fd = %d quit!", ws->_connects[fd].server_name().c_str(), fd);
	ws->_connects[fd].close();
	ws->_ioc.cancel(fd);
}




