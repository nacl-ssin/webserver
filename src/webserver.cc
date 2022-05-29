//
// Created by 26372 on 2022/4/17.
//

#include "webserver.h"
#include <utility>


Webserver::Webserver(Webserver::port_t port, bool is_et) : _lfd(-1), _port(port) {
	_trigger_mode = 0;
	if (is_et) {
		_trigger_mode |= EPOLLET;
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
	// 边沿触发时
	if (_trigger_mode & EPOLLET) {
		// 设置为非阻塞套接字
		if (!set_nonblock(_lfd)) {
			LOG_FATAL("set non block failed");
			exit(1);
		}
	}
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


void Webserver::start() {
	EventItem ei;
	ei._r = &_ioc;
	ei._ptr = this;
	if (_trigger_mode & EPOLLET) {
		ei._async = true;
	}
	ei.register_event(_lfd, accept_cb, nullptr, nullptr);
	_ioc.add_event(ei, EPOLLIN | _trigger_mode);
	_ioc.event_dispatch(-1);
}


void Webserver::accept_cb(EventItem &ei) {
	auto svr = static_cast<Webserver *>(ei._ptr);
	uint32_t mode = svr->_trigger_mode;
	do {
		sockaddr_in origin_addr = {};
		socklen_t origin_addr_len = sizeof(origin_addr);
		memset(&origin_addr, 0, sizeof(origin_addr));
		int cfd = accept(ei._fd, (sockaddr *) &origin_addr, &origin_addr_len);
		if (cfd > 0) {
			LOG_INFO("accept a new client fd = %d", cfd);

			if (HttpConnection::is_et()) {
				if (!set_nonblock(cfd)) {
					LOG_ERROR("set no block failed, fd = %d", cfd);
					close(cfd);
					continue;
				}
			}
			EventItem e;

			// ET模式，异步调用
			if (HttpConnection::is_et()) {
				e._async = true;
			}
			e._r = ei._r;
			e._ptr = ei._ptr;
			e.register_event(cfd, recv_cb, write_cb, error_cb);
			auto *svr = static_cast<Webserver *>(ei._ptr);
			svr->_connects[cfd] = HttpConnection(cfd, origin_addr);
			e._r->add_event(e, HttpConnection::_trigger_mode | EPOLLIN | EPOLLRDHUP);
		} else {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				//LOG_INFO("empty connection");
				return;
			}

			if (errno == EINTR) {
				continue;
			}

			LOG_WARRING("accept_cb client failed!");
			continue;
		}
	} while (mode & EPOLLET);
}


void Webserver::recv_cb(EventItem &ei) {
	LOG_INFO("recv from fd = %d client message", ei._fd);
	auto *svr = static_cast<Webserver *>(ei._ptr);
	if (svr->_connects.count(ei._fd) == 0) {
		return;
	}
	// 读取数据
	int r = svr->_connects[ei._fd].receive();

	if (r != 0) {
		LOG_ERROR("receive error fd = %d", ei._fd);
		ei._error(ei);
	} else {
		if (svr->_connects[ei._fd].is_close()) {
			ei._error(ei);
		} else {
			if (svr->_connects[ei._fd].ready() &&
				!ei._r->modify(ei._fd, EPOLLOUT | EPOLLRDHUP | HttpConnection::_trigger_mode)) {
				ei._error(ei);
			}
		}
	}
}


void Webserver::write_cb(EventItem &ei) {
	auto *svr = static_cast<Webserver *>(ei._ptr);
	auto &connection = svr->_connects[ei._fd];
	std::string url;
	// 之前还有未发送的数据
	LOG_FATAL("closed = %d, fd = %d, empty = %d", connection.is_close(), ei._fd,
			  connection._outbuffer.empty());
	if (!connection.is_close() && !connection._outbuffer.empty()) {
		if (-1 == connection.send_buffer()) {
			ei._error(ei);
			return;
		}
		goto send_end;
	}

	// 连接未关闭并且响应报文准备完毕
	if (!connection.is_close() && connection.ready()) {
		LOG_INFO("write call fd = %d, thread id = %ld", ei._fd, pthread_self());
		url = connection.get_method() + ":" + connection.get_path();
		if (svr->_events_map.find(url) != svr->_events_map.end()) {
			(svr->_events_map[url])(connection._request, connection._response);
			if (-1 == connection.send()) {
				ei._error(ei);
			}
		} else {
			if (-1 == connection.send_file()) {
				ei._error(ei);
			}
		}

		send_end:

		LOG_FATAL("closed = %d, fd = %d, empty = %d", connection.is_close(), ei._fd,
				  connection._outbuffer.empty());
		// 这次响应是否发送完毕
		if (connection._outbuffer.empty()) {
			LOG_WARRING("empty");
			if (connection.keep_alive()) {
				if (!svr->_ioc.modify(ei._fd, EPOLLIN | EPOLLRDHUP | HttpConnection::_trigger_mode)) {
					LOG_ERROR("modify error");
					ei._error(ei);
					return;
				}
				//继续解析报文
				connection.clear();
				connection.again_recv();
			} else {
				ei._error(ei);
			}
		}
	}
}


void Webserver::error_cb(EventItem &ei) {
	auto *svr = static_cast<Webserver *>(ei._ptr);
	if (svr->_connects.count(ei._fd) != 0) {
		auto &connection = svr->_connects[ei._fd];
		// 关闭连接
		LOG_INFO("server name = %s, fd = %d quit!", connection.server_name().c_str(), ei._fd);
		connection.close();
		svr->_connects.erase(ei._fd);
		svr->_ioc.cancel(ei._fd);
	}
}



