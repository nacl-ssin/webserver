//
// Created by 26372 on 2022/4/17.
//

#ifndef WEBSERVER_WEBSERVER_H
#define WEBSERVER_WEBSERVER_H

#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <functional>
#include <unordered_map>
#include "http_connection.h"
#include "thread_pool.hpp"
#include "epoller.h"
#include "logger.h"


class Webserver {
	using port_t = unsigned short;
	using request_trigger = std::function<void(HttpRequest &, HttpResponse &)>;

private:
	int _lfd;
	Epoller _ioc;
	port_t _port;
	thread_pool *_tp;
	std::unordered_map<int, HttpConnection> _connects;
	std::unordered_map<std::string, request_trigger> _events_map;

public:
	explicit Webserver(port_t port);

	Webserver(const Webserver &ws) = delete;

	Webserver &operator=(const Webserver &ws) = delete;

	~Webserver();

	/**
	 * 初始化webserver
	 */
	void init_server();

	/**
	 * 进行事件分发
	 */
	[[noreturn]] void dispatch();


	void get(const std::string &url, request_trigger trigger);


	void post(const std::string &url, request_trigger trigger);

private:
	 /**
	  * 接收请求处理
	  * @param ws
	  */
	void accept_cb();

	/**
	 * 响应回调
	 * @param fd
	 */
	static void recv_cb(Webserver *ws, int fd);

	/**
	 * 出错回调
	 * @param fd
	 */
	static void error_cb(Webserver *ws, int fd);
};


#endif //WEBSERVER_WEBSERVER_H
