//
// Created by 26372 on 2022/4/17.
//

#ifndef WEBSERVER_PROTOCOL_H
#define WEBSERVER_PROTOCOL_H

#include <string>
#include <vector>
#include <unordered_map>

struct HttpData {
	std::string _method;    /*方法*/
	std::string _version;   /*版本*/
	std::vector<std::pair<std::string, std::string>> _header;   /*请求头*/
	std::string _body;      /*正文*/
};

struct HttpRequest : public HttpData {
	std::string _path;                          /*请求路径*/
	std::string _query;
};


struct HttpResponse : public HttpData {
	std::string _code;                         /*响应状态码*/
	std::string _code_msg;                     /*响应状态码描述*/

	void set_header(std::string key, const std::size_t val) {
		_header.emplace_back(std::move(key), std::to_string(val));
	}

	void set_header(std::string key, std::string val) {
		_header.emplace_back(std::move(key), std::move(val));
	}
};

/**
 * 根据文件后缀名，返回content-type类型
 * @param suffix
 * @return
 */
std::string file_type(const std::string &suffix);

#endif //WEBSERVER_PROTOCOL_H
