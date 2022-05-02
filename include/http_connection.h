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
	bool _closed;
    sockaddr_in _addr;
    HttpRequest _request;
    HttpResponse _response;
	static const std::string RESOURCE_ROOT;

public:
	HttpConnection();

    HttpConnection(int sock, sockaddr_in addr);

	/**
	 * 获取客户端ip
	 * @return
	 */
	std::string server_name();

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


	/**
	 * 完整的资源路径
	 * @return
	 */
	inline std::string full_file_path() {
		return HttpConnection::RESOURCE_ROOT + _request._path;
	}
private:
	/**
	 * cgi处理
	 */
	void cgi_handler();
};



#endif //WEBSERVER_HTTP_CONNECTION_H
