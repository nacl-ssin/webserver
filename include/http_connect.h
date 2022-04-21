//
// Created by 26372 on 2022/4/17.
//

#ifndef WEBSERVER_HTTP_CONNECTION_H
#define WEBSERVER_HTTP_CONNECTION_H

#include <sstream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include "protocol.h"
#include "utils.h"
#include "log.h"

class HttpConnect {
private:
    int fd_;
    bool is_parsed_;
    sockaddr_in addr_;
    HttpRequest request_;
    HttpResponse response_;
public:
	HttpConnect() = default;

    HttpConnect(int sock, sockaddr_in addr);

    int get_fd() const;

	std::string server_name();

    ssize_t read();

    void write();
private:
    /**
     * 解析请求报文
     */
    void parse_data(std::string str);

    /**
     * 构建响应
     * @return
     */
    std::string build_response();
};

#endif //WEBSERVER_HTTP_CONNECTION_H
