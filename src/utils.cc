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
			if (start < src.size()) {
				ret.emplace_back(src.substr(start));
			}
			break;
		}
		ret.emplace_back(src.substr(start, pos));
		start = pos;
		start += rep.size();
	}
	return ret;
}
