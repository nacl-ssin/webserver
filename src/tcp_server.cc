//
// Created by 26372 on 2022/4/17.
//

#include "tcp_server.h"


TcpServer::TcpServer(TcpServer::port_t port) : port_(port), listen_sock_(-1) {
}

void TcpServer::init_server() {
    listen_sock_ = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock_ < 0) {
        LOG_FATAL("socket create error!");
        exit(1);
    }

    sockaddr_in local;
    memset(&local, 0, sizeof(local));
    local.sin_family = AF_INET;
    local.sin_port = htons(port_);
    local.sin_addr.s_addr = INADDR_ANY;

    if (bind(listen_sock_, (sockaddr*) &local, sizeof(local)) == -1) {
        LOG_FATAL("bind error!");
        exit(1);
    }

    if(listen(listen_sock_, 5) == -1) {
        LOG_FATAL("listen error!");
        exit(1);
    }
}

TcpServer::~TcpServer() {
    if (listen_sock_ >= 0) {
        close(listen_sock_);
    }
}

int TcpServer::get_lfd() const {
    return listen_sock_
}

