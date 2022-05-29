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
#include "epoller.h"
#include "logger.h"


class Webserver {
	using port_t = unsigned short;
	using request_trigger = std::function<void(HttpRequest &, HttpResponse &)>;

private:
	int _lfd;
	Epoller _ioc;
	port_t _port;
	uint32_t _trigger_mode;
	std::string _static_resource_root_path;
	std::unordered_map<int, HttpConnection> _connects;
	std::unordered_map<std::string, request_trigger> _events_map;

public:
	explicit Webserver(port_t port, bool is_et = true);

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
	void start();


	void get(const std::string &url, request_trigger trigger);


	void post(const std::string &url, request_trigger trigger);

	/**
	 * 设置静态资源的路径
	 * @param path
	 */
	void static_path(std::string static_path);

private:
	/**
	 * 接收新连接
	 * @param ei
	 */
	static void accept_cb(EventItem &ei);

	/**
	 * 响应回调
	 * @param ws
	 * @param fd
	 */
	static void recv_cb(EventItem &ei);

	/**
	 * 发送回调
	 * @param ws
	 * @param fd
	 */
	static void write_cb(EventItem &ei);


	/**
	 * 出错回调
	 * @param fd
	 */
	static void error_cb(EventItem &ei);
};


#endif //WEBSERVER_WEBSERVER_H
