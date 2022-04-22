//
// Created by 26372 on 2022/4/17.
//

#ifndef WEBSERVER_HTTP_CONNECTION_H
#define WEBSERVER_HTTP_CONNECTION_H

#include <sstream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <sys/sendfile.h>
#include "protocol.h"
#include "utils.h"
#include "log.h"

class HttpConnect {
private:
    int _fd;
    sockaddr_in _addr;
    HttpRequest _request;
    HttpResponse _response;
	static const std::string RESOURCE_ROOT;
public:
	HttpConnect();

    HttpConnect(int sock, sockaddr_in addr);

    int get_fd() const;

	/**
	 * 获取客户端ip
	 * @return
	 */
	std::string server_name();

	/**
	 * 读取数据
	 * @return
	 */
    ssize_t read();

	/**
	 * 发送数据
	 */
    void write();
private:
    /**
     * 解析请求报文
     */
    void parse_data(const std::string& str);

    /**
     * 构建响应字符串
     * @return
     */
    std::string build_response();
};


#endif //WEBSERVER_HTTP_CONNECTION_H
