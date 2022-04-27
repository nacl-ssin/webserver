//
// Created by 26372 on 2022/4/17.
//

#include "webserver.h"
#include "thread_pool.hpp"

Webserver::Webserver(Webserver::port_t port) : _lfd(-1), _port(port), _tp(nullptr) {
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

	LOG_INFO("create socket success is %d", _lfd);
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
		LOG_ERROR("thread pool create error!");
		exit(3);
	}
	_tp->start(5);
	// 忽略掉SIGPIPE信号

	//for (int i = 1; i < 32; ++i) {
	signal(SIGPIPE, SIG_IGN);
	//}
	LOG_INFO("server start on 8080 port");
}


[[noreturn]] void Webserver::dispatch() {
	_ioc.add_event(_lfd, EPOLLIN | EPOLLET);

	while (true) {
		int nready = _ioc.wait(-1);
		auto *ready = _ioc.get_events();
		//LOG_INFO("nread = %d", nready);
		for (int i = 0; i < nready; ++i) {
			//sleep(1);
			epoll_event ev = ready[i];
			if (ev.data.fd == _lfd && (ev.events & EPOLLIN)) {
				// 接收新连接
				_tp->push_task(accept_cb, this);
				//accept_cb(this);
			}

			if (ev.data.fd != _lfd && (ev.events & EPOLLIN)) {
				int fd = ev.data.fd;
				_tp->push_task(recv_cb, this, fd);
				//recv_cb(this, ev.data.fd);
			}

			//if (ev.events & EPOLLHUP) {
			//	LOG_WARRING("error");
			//    // 出错
			//	int fd = ev.data.fd;
			//	_tp->push_task(error_cb, this, fd);
			//	//error_cb(this, ev.data.fd);
			//}
		}
	}
}


void Webserver::accept_cb(Webserver *ws) {
	sockaddr_in origin_addr = {};
	socklen_t origin_addr_len = sizeof(origin_addr);
	memset(&origin_addr, 0, sizeof(origin_addr));
	int cfd = accept(ws->_lfd, (sockaddr *) &origin_addr, &origin_addr_len);
	if (cfd == -1) {
		LOG_WARRING("accept_cb client failed! cause: ", strerror(errno));
		return;
	}
	LOG_INFO("accept a new client fd = %d", cfd);

	ws->_ioc.add_event(cfd, EPOLLIN | EPOLLET);
	ws->_connects.insert(std::make_pair(cfd, HttpConnection(cfd, origin_addr)));
	LOG_INFO("accept end");
}


void Webserver::recv_cb(Webserver *ws, int fd) {
	LOG_INFO("recv from fd = %d client message", fd);
	// 读取数据
	ssize_t nread = ws->_connects[fd].receive();

	if (nread <= 0) {
		LOG_WARRING("nread = %d", nread);
		error_cb(ws, fd);
	} else {
		ws->_connects[fd].send();
	}
	LOG_INFO("recv end");
}


// ERROR
void Webserver::error_cb(Webserver *ws, int fd) {
	LOG_ERROR("error fd = %d", fd);
	// 删除连接
	// 不删出连接，下一次来连接，直接覆盖

	ws->_ioc.cancel(fd);
	//LOG_INFO("quit ret = %d", ret);
	close(fd);
	LOG_INFO("error end");
}


