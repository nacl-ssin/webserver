//
// Created by 26372 on 2022/4/17.
//

#ifndef WEBSERVER_TCP_SERVER_H
#define WEBSERVER_TCP_SERVER_H

#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "log.h"


class TcpServer {
public:
    using port_t = unsigned short;
private:
    port_t port_;
    int listen_sock_;

public:
    TcpServer() = default;

    explicit TcpServer(port_t port);

    ~TcpServer();

    void init_server();

    int get_lfd() const;
};

#endif //WEBSERVER_TCP_SERVER_H
