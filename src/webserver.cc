//
// Created by 26372 on 2022/4/17.
//

#include "webserver.h"

Webserver::Webserver(Webserver::port_t port) : lfd_(-1), port_(port) {
    init_server();
}

Webserver::~Webserver() {
    if (lfd_ >= 0) {
        close(lfd_);
    }
}

void Webserver::init_server() {
    lfd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (lfd_ < 0) {
        LOG_FATAL("socket create error!");
        exit(1);
    }

    LOG_INFO("create socket success is %d", lfd_);
    // 设置端口复用
    int opt = 1;
    setsockopt(lfd_, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

    sockaddr_in local = {};
    memset(&local, 0, sizeof(local));
    local.sin_family = AF_INET;
    local.sin_port = htons(port_);
    local.sin_addr.s_addr = INADDR_ANY;

    if (bind(lfd_, (sockaddr*) &local, sizeof(local)) == -1) {
        LOG_FATAL("bind error!");
        exit(1);
    }

    if(listen(lfd_, 128) == -1) {
        LOG_FATAL("listen error!");
        exit(1);
    }
    LOG_INFO("server start on 8080 port");
}


[[noreturn]] void Webserver::dispatch() {
    ioc_.add_event(lfd_, EPOLLIN);

    while (true) {
        int nready = ioc_.wait(-1);
        auto *ready = ioc_.get_events();
        for (int i = 0; i < nready; ++i) {
            epoll_event ev = ready[i];
            if (ev.data.fd == lfd_ && (ev.events & EPOLLIN)) {
                // 可读
                accept_cb();
            }

            if (ev.data.fd != lfd_ && (ev.events & EPOLLIN)) {
                recv_cb(ev.data.fd);
            }

            //if (ev.events & EPOLLOUT) {
            //    // 可写
			//	if (connects_.find(ev.data.fd) != connects_.end()) {
			//		connects_[ev.data.fd].write();
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
    sleep(1);
    sockaddr_in origin_addr = {};
    socklen_t origin_addr_len = sizeof(origin_addr);
    memset(&origin_addr, 0, sizeof(origin_addr));
    int cfd = accept(lfd_, (sockaddr *) &origin_addr, &origin_addr_len);
    if (cfd == -1) {
        LOG_WARRING("accept_cb client failed! cause: ", strerror(errno));
        return;
    }
    LOG_INFO("get a new client fd = %d", cfd);

    ioc_.add_event(cfd, EPOLLIN);
    connects_.insert(std::make_pair(cfd, HttpConnect(cfd, origin_addr)));
}

void Webserver::recv_cb(int fd) {
    sleep(1);
    LOG_INFO("recv from client message fd = %d", fd);
	// 读取数据
	ssize_t nread = connects_[fd].read();
	if (nread == 0) {
		error_cb(fd);
	}
	connects_[fd].write();
}


void Webserver::error_cb(int fd) {
	// 删除连接
	LOG_INFO("client quit server name = %s", connects_[fd].server_name().c_str());
    connects_.erase(fd);
	ioc_.cancel(fd);
	close(fd);
}


