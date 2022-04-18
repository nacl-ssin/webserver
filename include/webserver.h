//
// Created by 26372 on 2022/4/17.
//

#ifndef WEBSERVER_WEBSERVER_H
#define WEBSERVER_WEBSERVER_H

#include <unordered_map>
#include "tcp_server.h"
#include "http_connect.h"

class Webserver {
    using port_t = TcpServer::port_t;
private:
    TcpServer tcp_svr_;
    std::unordered_map<int, HttpConnect> connects_;

public:
    explicit Webserver(port_t port);

    void start();

    [[noreturn]] void accept();
};


#endif //WEBSERVER_WEBSERVER_H
