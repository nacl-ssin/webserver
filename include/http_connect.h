//
// Created by 26372 on 2022/4/17.
//

#ifndef WEBSERVER_HTTP_CONNECTION_H
#define WEBSERVER_HTTP_CONNECTION_H

#include <arpa/inet.h>
#include "protocol.h"

class HttpConnect {
private:
    int sock_;
    sockaddr_in addr_;
    HttpRequest request_;
    HttpResponse response_;
    iovec i;
public:
    HttpConnect(int sock, sockaddr_in addr);
};

#endif //WEBSERVER_HTTP_CONNECTION_H
