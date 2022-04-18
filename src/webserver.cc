//
// Created by 26372 on 2022/4/17.
//

#include "webserver.h"

Webserver::Webserver(Webserver::port_t port) : tcp_svr_(port) {

}

void Webserver::start() {
    tcp_svr_.init_server();
}

[[noreturn]] void Webserver::accept() {
    sockaddr_in origin_addr;
    socklen_t origin_addr_len = sizeof(origin_addr);
    while (true) {
        memset(&origin_addr, 0, sizeof(origin_addr));
        int cfd = ::accept(tcp_svr_.get_lfd(), (sockaddr*) &origin_addr, &origin_addr_len);
        if (cfd == -1) {
            LOG_WARRING("accept client failed!");
            continue;
        }
        connects_.insert(std::make_pair(cfd, HttpConnect(cfd, origin_addr)));
    }
}

