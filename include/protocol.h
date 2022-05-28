//
// Created by 26372 on 2022/4/17.
//

#ifndef WEBSERVER_PROTOCOL_H
#define WEBSERVER_PROTOCOL_H

#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include "utils.h"
#include "logger.h"

struct HttpData {
protected:
	std::string _method;                                        /*方法*/
	std::string _version;                                    /*版本*/
	std::vector<std::pair<std::string, std::string>> _header;   /*请求头*/
	std::string _body;                                        /*正文*/
};


class HttpRequest : public HttpData {
	friend class HttpConnection;

	enum class parse_state {
		LINE,
		HEADERS,
		BODY,
		READY
	};

private:
	std::string _path;                                        /*请求路径*/
	std::string _query;                                            /*请求参数*/
	parse_state _state;
	std::vector<std::pair<std::string, std::string>> _params;   /*解析后的请求参数*/

public:

	HttpRequest();

	/**
	 * 解析请求报文
	 * @param request_str
	 */
	//void parse(const std::string &request_str);


	/**
	 * 解析报文
	 * @param rd_buf
	 */
	void parse(std::string &buffer);


	/**
	 * 解析请求行
	 * @param rd_buf
	 * @return
	 */
	bool parse_line(std::string &buffer);

	/**
	 * 解析请求头
	 * @param rd_buf
	 * @return
	 */
	bool parse_headers(std::string &buffer);

	/**
	 * 解析请求体
	 * @param rd_buf
	 * @return
	 */
	bool parse_body(std::string &buffer);

	/**
	 * 获取请求路径
	 * @return
	 */
	std::string &get_path();

	/**
	 * 获取请求参数
	 * @param key
	 * @return
	 */
	std::string get_params(const std::string &key);
};


class HttpResponse : public HttpData {
	friend class HttpConnection;

private:
	std::string _code;                         /*响应状态码*/
	std::string _code_msg;                     /*响应状态码描述*/

public:
	/**
	 * 设置响应头
	 * @param key
	 * @param val
	 */
	HttpResponse &set_header(std::string key, std::size_t val);

	HttpResponse &set_header(std::string key, std::string val);

private:

	/**
	 * 设置响应行
	 * @param version
	 * @param code
	 * @param code_msg
	 */
	HttpResponse &set_response_line(std::string version, int code, std::string code_msg);

	HttpResponse &set_response_line(int code, std::string code_msg);

public:

	/**
	 * 设置响应正文和Content-Length
	 * @param body
	 */
	HttpResponse &set_body(const std::string &body);

	/**
	 * 设置响应状态码+状态码描述
	 * @param code
	 */
	HttpResponse &set_code(int code);


	/**
	 * 构建响应报文
	 * @return
	 */
	std::string build();
};


/**
 * 根据文件后缀获取content-type
 * @param suffix
 * @return
 */
std::string file_type(const std::string &suffix);


#endif //WEBSERVER_PROTOCOL_H
