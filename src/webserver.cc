//
// Created by 26372 on 2022/4/17.
//

#include "webserver.h"

Webserver::Webserver(Webserver::port_t port) : _lfd(-1), _port(port) {
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

    if (bind(_lfd, (sockaddr*) &local, sizeof(local)) == -1) {
        LOG_FATAL("bind error!");
        exit(1);
    }

    if(listen(_lfd, 128) == -1) {
        LOG_FATAL("listen error!");
        exit(1);
    }
    LOG_INFO("server start on 8080 port");
}


[[noreturn]] void Webserver::dispatch() {
    _ioc.add_event(_lfd, EPOLLIN);

    while (true) {
        int nready = _ioc.wait(-1);
        auto *ready = _ioc.get_events();
        for (int i = 0; i < nready; ++i) {
            epoll_event ev = ready[i];
            if (ev.data.fd == _lfd && (ev.events & EPOLLIN)) {
                // 可读
                accept_cb();
            }

            if (ev.data.fd != _lfd && (ev.events & EPOLLIN)) {
                recv_cb(ev.data.fd);
            }

            //if (ev.events & EPOLLOUT) {
            //    // 可写
			//	if (_connects.find(ev.data.fd) != _connects.end()) {
			//		_connects[ev.data.fd].write();
			//		LOG_INFO("can write to");
			//	}
            //}

            if (ev.events & EPOLLHUP) {
                // 出错
                error_cb(ev.data.fd);
            }
        }
    }
}

void Webserver::accept_cb() {
    sockaddr_in origin_addr = {};
    socklen_t origin_addr_len = sizeof(origin_addr);
    memset(&origin_addr, 0, sizeof(origin_addr));
    int cfd = accept(_lfd, (sockaddr *) &origin_addr, &origin_addr_len);
    if (cfd == -1) {
        LOG_WARRING("accept_cb client failed! cause: ", strerror(errno));
        return;
    }
    LOG_INFO("accept a new client fd = %d", cfd);

    _ioc.add_event(cfd, EPOLLIN);
    _connects.insert(std::make_pair(cfd, HttpConnect(cfd, origin_addr)));
}

void Webserver::recv_cb(int fd) {
    LOG_INFO("recv from fd = %d client message", fd);
	// 读取数据
	ssize_t nread = _connects[fd].read();

	if (nread <= 0) {
		error_cb(fd);
	} else {
		_connects[fd].write();
	}
}

// ERROR
void Webserver::error_cb(int fd) {
	// 删除连接
	LOG_INFO("client quit server name = %s", _connects[fd].server_name().c_str());
    _connects.erase(fd);
	bool ret = _ioc.cancel(fd);
	LOG_INFO("quit ret = %d", ret);
	close(fd);
}


