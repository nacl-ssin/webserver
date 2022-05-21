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

class HttpConnection {
	friend class Webserver;
private:
    int _fd;
	bool _ready;
	bool _closed;
    sockaddr_in _addr;
    HttpRequest _request;
    HttpResponse _response;
	static std::string _static_resource_root_path;

public:
	HttpConnection();

    HttpConnection(int sock, sockaddr_in addr);

	/**
	 * 获取客户端ip
	 * @return
	 */
	std::string server_name();

	/**
	 * 是否准备好响应报文
	 * @return
	 */
	bool ready() const;

	/**
	 * 读取数据
	 * @return
	 */
    ssize_t receive();

	/**
	 * 发送数据
	 */
    void send_file();

	/**
	 * 构建响应报文发送
	 */
	void send();

	/**
	 * 未知的资源
	 */
	void send_error();

	/**
	 * 是否是长连接
	 * @return
	 */
	bool keep_alive();

	/**
	 * 关闭连接
	 */
	inline void close() {
		_closed = true;
		::close(_fd);
	}

	/**
	 * 是否关闭对象
	 * @return
	 */
	inline bool is_close() const {
		return _closed;
	}

	/**
	 * 获取请求方法
	 * @return
	 */
	inline std::string &get_method() {
		return _request._method;
	}


	inline std::string &get_path() {
		return _request._path;
	}

	inline std::string get_resource_full_path() {
		return _static_resource_root_path + _request._path;
	}

private:
	/**
	 * cgi处理
	 */
	void cgi_handler();
};



#endif //WEBSERVER_HTTP_CONNECTION_H
