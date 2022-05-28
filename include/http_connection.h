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
#include <sys/sendfile.h>
#include <sys/wait.h>
#include <sys/epoll.h>
#include "protocol.h"
#include "buffer.h"

class HttpConnection {
	friend class Webserver;

private:
	int _fd;
	bool _closed;
	std::string _inbuffer;
	Buffer _outbuffer;
	sockaddr_in _addr;
	HttpRequest _request;
	HttpResponse _response;
	static uint32_t _trigger_mode;
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
	 * 读取数据
	 * @return
	 */
	int receive();

	/**
	 * 发送数据
	 */
	int send_file();

	/**
	 * 构建响应报文发送
	 */
	int send();

	/**
	 * 发送缓冲区的内容
	 */
	int send_buffer();

	/**
	 * 未知的资源
	 */
	int send_error();

	/**
	 * 是否是长连接
	 * @return
	 */
	bool keep_alive();

	/**
	 * 继续接受报文
	 */
	void again_recv() {
		_request._state = HttpRequest::parse_state::LINE;
	}

	/**
	 * 关闭连接
	 */
	inline void close() {
		_closed = true;
		_inbuffer.clear();
		_outbuffer.clear();
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


	/**
	 * 获取请求路径
	 * @return
	 */
	inline std::string &get_path() {
		return _request._path;
	}

	/**
	 * 获取文件资源的路径
	 * @return
	 */
	inline std::string get_resource_full_path() {
		return _static_resource_root_path + _request._path;
	}


	/**
	 * 是否准备好响应报文
	 * @return
	 */
	bool ready() const {
		return _request._state == HttpRequest::parse_state::READY;
	}


	/**
	 * 是否是EPOOLET触发模式
	 * @return
	 */
	static bool is_et() {
		return (HttpConnection::_trigger_mode & EPOLLET);
	}

private:
	/**
	 * cgi处理
	 */
	void cgi_handler();
};


#endif //WEBSERVER_HTTP_CONNECTION_H
