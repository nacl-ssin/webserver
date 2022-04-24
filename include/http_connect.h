//
// Created by 26372 on 2022/4/17.
//

#ifndef WEBSERVER_HTTP_CONNECTION_H
#define WEBSERVER_HTTP_CONNECTION_H

#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <sys/sendfile.h>
#include <sys/wait.h>
#include "protocol.h"

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
	 * cgi处理
	 */
	void cgi_handler();

};


#endif //WEBSERVER_HTTP_CONNECTION_H
