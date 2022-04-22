//
// Created by 26372 on 2022/4/17.
//

#ifndef WEBSERVER_WEBSERVER_H
#define WEBSERVER_WEBSERVER_H

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unordered_map>
#include "http_connect.h"
#include "iocntl.h"
#include "log.h"


class Webserver {
	using port_t = unsigned short;
private:
	int _lfd;
	IOCntl _ioc;
	port_t _port;
	std::unordered_map<int, HttpConnect> _connects;

public:
	explicit Webserver(port_t port);

	~Webserver();

	/**
	 * 初始化webserver
	 */
	void init_server();

	/**
	 * 进行事件分发
	 */
	[[noreturn]] void dispatch();

private:
	/**
	 * 接收请求处理
	 */
	void accept_cb();

	/**
	 * 响应回调
	 * @param fd
	 */
	void recv_cb(int fd);

	/**
	 * 出错回调
	 * @param fd
	 */
	void error_cb(int fd);
};


#endif //WEBSERVER_WEBSERVER_H
