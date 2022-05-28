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


bool read_line(std::string &src, std::string &line) {
	int n = src.size();
	for (int i = 0; i < n; ++i) {
		if (src[i] == '\n') {
			line = src.substr(0, i);
			src = src.substr(i + 1);
			return true;
		}
		if (src[i] == '\r') {
			if (i + 1 < n && src[i + 1] == '\n') {
				++i;
				line = src.substr(0, i - 1);
			} else {
				line = src.substr(0, i);
			}
			src = src.substr(i + 1);
			return true;
		}
	}
	return false;
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

std::string to_upper(const std::string &str) {
	std::string ret;
	ret.reserve(str.size());
	for (auto c : str) {
		if (std::isalpha(c)) {
			ret.push_back(toupper(c));
		}
	}
	return ret;
}


bool strcmpi(const std::string &str1, const std::string &str2) {
	return to_upper(str1) == to_upper(str2);
}

bool set_nonblock(int fd) {
	return fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0) | O_NONBLOCK) == 0;
}

