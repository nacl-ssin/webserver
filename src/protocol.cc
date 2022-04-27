//
// Created by 26372 on 2022/4/17.
//

#include "protocol.h"

// Request--------------------------------------------------------------------------------


void HttpRequest::parse(const std::string &request_str) {
	std::string line;
	size_t pos = 0;
	while (pos < request_str.size()) {
		auto p = read_line(request_str, pos);
		line = p.first;
		//LOG_INFO("line = %s", line.c_str());
		if (pos == 0) {
			// 解析请求行
			std::stringstream ss(line);
			ss >> _method >> _path >> _version;
			// 将请求地址和参数分离，如果有的话
			auto v = split(_path, "?");
			_path = std::move(v[0]);
			if (v.size() > 1) {
				_query = std::move(v[1]);
			}
		} else {
			// 解析请求头
			std::vector<std::string> ret = split(line, ": ");
			//for (auto &s : ret) {
				//LOG_INFO("header = %s", s.c_str());
			//}
			if (ret.size() == 2) {
				_header.emplace_back(std::make_pair(ret[0], ret[1]));
			}
		}
		pos = p.second + 1;

		if (line.empty()) {
			// 到了请求正文了
			p = read_line(request_str, pos);
			_body = p.first;
			//LOG_INFO("request body = %s", p.first.c_str());
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


// Response--------------------------------------------------------------------------------

void HttpResponse::set_header(std::string key, std::size_t val) {
	_header.emplace_back(std::move(key), std::to_string(val));
}


void HttpResponse::set_header(std::string key, std::string val) {
	_header.emplace_back(std::move(key), std::move(val));
}


void HttpResponse::set_response_line(std::string version, int code, std::string code_msg) {
	_version = std::move(version);
	_code = std::to_string(code);
	_code_msg = std::move(code_msg);
}


void HttpResponse::set_response_line(int code, std::string code_msg) {
	set_response_line("HTTP/1.0", code, std::move(code_msg));
}


void HttpResponse::set_body(const std::string &body) {
	_body = body;
	set_header("Content-Length", _body.size());
}


void HttpResponse::set_code(int code) {
	const static std::unordered_map<int, std::string> CODE_MSG {
			{200, "OK"},
			{300, "Multiple Choices"},
			{301, "Moved Permanently"},
			{400, "Bad Request"},
			{404, "Not Found"},
			{500, "Internal Server Error"},
			{501, "Not Implemented"},
	};
	auto iter = CODE_MSG.find(code);
	if (iter != CODE_MSG.end()) {
		set_response_line(code, iter->second);
	} else {
		set_response_line(code, "error");
	}
}


std::string HttpResponse::build() {
	std::stringstream ss;
	// 响应行
	ss << _version << " " << _code << " " << _code_msg << "\n";
	// 响应头
	for (auto &p : _header) {
		ss << p.first << ": " << p.second << "\n";
	}
	// 响应空行
	ss << "\n";
	// 响应正文
	ss << _body;
	return ss.str();
}






//Other------------------------------------------------------------------------------------




const static std::unordered_map<std::string, std::string> FILE_SUFFIX_MAPPED = {
		{"html",  "text/html"},
		{"htm",   "text/html"},
		{"css",   "text/css"},
		{"xml",   "application/xml"},
		{"json",  "application/json"},
		{"js",    "application/javascript"},
		{"mpa",   "video/mpeg"},
		{"png",   "image/png"},
		{"jpeg",  "image/jpeg"},
		{"woff2", "application/font-woff2"},
		{"txt",   "text/plain"},
		{"ppt",   "application/vnd.ms-powerpoint"},
		{"pic",   "image/pict"},
		{"body",  "text/html"},
		{"tif",   "image/tiff"},
		{"wbmp",  "image/vnd.wap.wbmp"},
		{"svgz",  "image/svg+xml"},
		{"woff",  "application/font-woff"},
		{"gif",   "image/gif"},
		{"jpg",   "image/jpeg"},
		{"ttf",   "application/x-font-ttf"},
		{"mp4",   "video/mp4"},
		{"mp3",   "audio/mpeg"},
		{"pdf",   "application/pdf"},
		{"xls",   "application/vnd.ms-excel"}
};


std::string file_type(const std::string &suffix) {
	auto iter = FILE_SUFFIX_MAPPED.find(suffix);
	return iter == FILE_SUFFIX_MAPPED.end() ? "text/plain" : iter->second;
}
