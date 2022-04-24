//
// Created by 26372 on 2022/4/17.
//

#include "utils.h"


std::string cur_time() {
	time_t t;
	time(&t);
	tm *ret = localtime(&t);
	char buf[20] = {0};
	sprintf(buf, "%d-%02d-%02d %02d:%02d:%02d", ret->tm_year - 100, ret->tm_mon + 1, ret->tm_mday, ret->tm_hour,
			ret->tm_min, ret->tm_sec);
	return buf;
}


std::pair<std::string, size_t> read_line(const std::string &src, size_t start = 0) {
	char ch = 'E';
	std::string line;
	for (; start < src.size(); ++start) {
		ch = src[start];
		if (ch == '\n') {
			break;
		} else if (ch == '\r') {
			if (start < src.size()) {
				ch = src[start + 1];
				if (ch == '\n') {
					start++;
				}
			}
			break;
		}
		line.push_back(ch);
	}
	return {line, start};
}


std::vector<std::string> split(const std::string &src, const std::string &rep) {
	std::vector<std::string> ret;
	size_t start = 0;
	while (true) {
		size_t pos = src.find(rep, start);
		if (pos == std::string::npos) {
			ret.emplace_back(src.substr(start));
			break;
		}
		ret.emplace_back(src.substr(start, pos - start));
		start = pos;
		start += rep.size();
	}
	return ret;
}


const static std::unordered_map<std::string, std::string> file_suffix_mapped = {
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
	auto iter = file_suffix_mapped.find(suffix);
	return iter == file_suffix_mapped.end() ? "text/plain" : iter->second;
}
