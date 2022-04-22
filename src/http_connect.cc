//
// Created by 26372 on 2022/4/17.
//

#include "http_connect.h"


//const std::string HttpConnect::RESOURCE_ROOT = "../static";
const std::string HttpConnect::RESOURCE_ROOT = "/home/nacl/remote_proj/clion/webserver/static";

HttpConnect::HttpConnect() : _fd(-1), _addr() {

}

HttpConnect::HttpConnect(int sock, sockaddr_in addr) : _fd(sock), _addr(addr) {

}

int HttpConnect::get_fd() const {
	return _fd;
}

std::string HttpConnect::server_name() {
	std::string ip(16, 0);
	inet_ntop(AF_INET, &_addr.sin_addr.s_addr, &ip[0], 16);
	return ip;
}

ssize_t HttpConnect::read() {
	char buf[1024] = {0};
	memset(buf, 0, sizeof(buf));
	ssize_t nread = recv(_fd, buf, sizeof(buf) - 1, 0);
	if (nread < 0) {
		LOG_ERROR("read error fd = %d", _fd);
		return nread;
	}

	if (nread == 0) {
		LOG_INFO("nread = %d", nread);
		return nread;
	}
	buf[nread] = 0;
	parse_data(buf);
	return nread;
}


void HttpConnect::write() {
	_response._version = _request._version;

	LOG_INFO("request path = %s", _request._path.c_str());

	// 是否打开了文件
	bool is_open = false;
	// 文件描述符
	int fd = -1;

	// 获取请求资源的路径
	std::string path = RESOURCE_ROOT + _request._path;
	// 获取文件属性
	struct stat st = {};
	if (stat(path.c_str(), &st) != 0) {
		// 路径不存在
		LOG_ERROR("stat error, cause: %s", strerror(errno));
		_response._code = std::to_string(404);
		_response._code_msg = "Not Found";
		_response._body = _request._path + " Not Found!";
		_response.set_header("Content-Length", _response._body.size());
	} else {
		// 检测是什么资源
		if (S_ISDIR(st.st_mode)) {
			// 这是一个目录
			if (_request._path == "/") {
				_request._path = "/index.html";
				goto RD_FILE;
			}
			_response._code = std::to_string(404);
			_response._code_msg = "Not Found";
			_response._body = _request._path + " Not Found!";
			_response.set_header("Content-Length", _response._body.size());
		} else if ((st.st_mode & S_IXUSR) || (st.st_mode & S_IXGRP) || (st.st_mode & S_IXOTH)) {
			// 是一个可执行文件，特殊处理

		} else {
			// 是一个文件，打开文件准备响应正文
			RD_FILE:
			fd = open((RESOURCE_ROOT + _request._path).c_str(), O_RDONLY);
			if (fd >= 0) {
				_response._code = std::to_string(200);
				_response._code_msg = "OK";
				_response.set_header("Content-Type", file_type(_request._path.substr(_request._path.rfind('.') + 1)));
				_response.set_header("Content-Length", st.st_size);
				is_open = true;
			} else {
				// 文件打开失败
				LOG_INFO("open [%s] error!, cause: %s", (RESOURCE_ROOT + _request._path).c_str(), strerror(errno));
				_response._code = std::to_string(500);
				_response._code_msg = "server error";
				_response._body = "server failed!";
				_response.set_header("Content-Length", _response._body.size());
			}
		}
	}

	std::string res = build_response();
	// 先发送响应行、报头、空行
	send(_fd, res.c_str(), res.size(), 0);

	LOG_INFO("response = %s", res.c_str());

	// 文件打开成功！
	// 改接口可以直接将文件的数据直接拷贝到网卡缓冲区中，就不用先拷贝到用户缓冲区，然后再拷贝至网卡缓冲区，少了一次拷贝
	if (is_open) {
		sendfile(_fd, fd, nullptr, st.st_size);
		close(fd);
	}
}


void HttpConnect::parse_data(const std::string &str) {
	std::string line;
	size_t pos = 0;
	while (pos < str.size()) {
		auto p = read_line(str, pos);
		line = p.first;
		LOG_INFO("line = %s", line.c_str());
		if (pos == 0) {
			// 解析请求行
			std::stringstream ss(line);
			ss >> _request._method >> _request._path >> _request._version;
			// 将请求地址和参数分离，如果有的话
			auto v = split(_request._path, "?");
			_request._path = v[0];
			if (v.size() > 1) {
				_request._query = v[1];
			}
		} else {
			// 解析请求头
			std::vector<std::string> ret = split(line, ": ");
			if (ret.size() == 2) {
				_request._header.emplace_back(std::make_pair(ret[0], ret[1]));
			}
		}
		pos = p.second + 1;

		if (line.empty()) {
			// 到了请求正文了
			p = read_line(str, pos);
			_request._body = p.first;
			LOG_INFO("request body = %s", p.first.c_str());
			pos = p.second + 1;
		}
	}

	LOG_INFO("parse data ending...");

	//LOG_INFO("method = %s url = %s version = %s", _request._method.c_str(), _request._path.c_str(),
	//		 _request._version.c_str());
	//
	//for (auto &p : _request._header) {
	//	LOG_INFO("key = %s, val = %s", p.first.c_str(), p.second.c_str());
	//}
}


std::string HttpConnect::build_response() {
	std::stringstream ss;
	// 响应行
	ss << _response._version << " " << _response._code << " " << _response._code_msg << "\n";
	// 响应头
	for (auto &p : _response._header) {
		ss << p.first << ": " << p.second << "\n";
	}
	// 响应空行
	ss << "\n";
	// 响应正文
	ss << _response._body;
	return ss.str();
}










