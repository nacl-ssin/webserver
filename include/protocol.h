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
#include "log.h"

struct HttpData {
protected:
	std::string _method;    /*方法*/
	std::string _version;   /*版本*/
	std::vector<std::pair<std::string, std::string>> _header;   /*请求头*/
	std::string _body;      /*正文*/
};


class HttpRequest : public HttpData {
	friend class HttpConnect;

private:
	std::string _path;                          /*请求路径*/
	std::string _query;                         /*请求参数*/

	/**
	 * 解析请求报文
	 * @param request_str
	 */
	void parse(const std::string &request_str);
};


class HttpResponse : public HttpData {
	friend class HttpConnect;

private:
	std::string _code;                         /*响应状态码*/
	std::string _code_msg;                     /*响应状态码描述*/

	/**
	 * 设置响应头
	 * @param key
	 * @param val
	 */
	void set_header(std::string key, std::size_t val);

	void set_header(std::string key, std::string val);

	/**
	 * 设置响应行
	 * @param version
	 * @param code
	 * @param code_msg
	 */
	void set_response_line(std::string version, int code, std::string code_msg);

	void set_response_line(int code, std::string code_msg);

	/**
	 * 构建响应报文
	 * @return
	 */
	std::string build();
};

#endif //WEBSERVER_PROTOCOL_H
