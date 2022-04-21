//
// Created by 26372 on 2022/4/17.
//

#include "http_connect.h"


HttpConnect::HttpConnect(int sock, sockaddr_in addr) : fd_(sock), addr_(addr), is_parsed_(false) {

}

int HttpConnect::get_fd() const {
    return fd_;
}

std::string HttpConnect::server_name() {
	std::string ip(16, 0);
	inet_ntop(AF_INET, &addr_.sin_addr.s_addr, &ip[0], 16);
	return ip;
}

ssize_t HttpConnect::read() {
	char buf[1024] = {0};
	memset(buf, 0, sizeof(buf));
	ssize_t nread = recv(fd_, buf, sizeof(buf) - 1, 0);
	if (nread < 0) {
		LOG_ERROR("read error fd = %d", fd_);
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
	std::string ret = "HTTP/1.0 200 OK\n";
    ret += "Content-Type: utf-8\n";
    ret += "Content-Length: 11\n";
    ret += "\n";
    ret += "Hello World\n";

    if (is_parsed_) {
        std::string res = build_response();
        send(fd_, ret.c_str(), ret.size(), 0);
        is_parsed_ = false;
    }
}

void HttpConnect::parse_data(std::string str) {
	std::string line;
	size_t pos = 0;
	while (pos < str.size()) {
		auto p = read_line(str, pos);
		line = p.first;
		LOG_INFO("line = %s", line.c_str());
		if (pos == 0) {
			std::stringstream ss(line);
			ss >> request_.method_ >> request_.url_ >> request_.version_;
		} else {
			std::vector<std::string> ret = split(line, ": ");
			if (ret.size() == 2) {
				request_.header_.insert(std::make_pair(ret[0], ret[1]));
			}
		}
		pos = p.second + 1;

		if (line.empty()) {
			p = read_line(str, pos);
			request_.body_ = p.first;
			LOG_INFO("request body = %s", p.first.c_str());
			pos = p.second + 1;
		}
	}

	LOG_INFO("parse data ending...");

	LOG_INFO("method = %s url = %s version = %s", request_.method_.c_str(), request_.url_.c_str(), request_.version_.c_str());

	for (auto &p : request_.header_) {
		LOG_INFO("key = %s, val = %s", p.first.c_str(), p.second.c_str());
	}

    is_parsed_ = true;
}

std::string HttpConnect::build_response() {
    if (!is_parsed_) return "";
    std::stringstream ss;
    // 响应行
    ss << response_.version_ << response_.code_ << response_.code_msg_ << "\n";
    // 响应头
    for (auto& p : response_.header_) {
        ss << p.first << ": " << p.second << "\n";
    }
    // 响应空行
    ss << "\n";
    // 响应正文
    ss << response_.body_;
    return ss.str();
}








