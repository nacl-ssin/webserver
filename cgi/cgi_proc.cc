#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <unistd.h>

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


int main() {
	int len = atoi(getenv("CONTENT_LEN"));

	char buf[len];

	int nrd = read(0, buf, sizeof(buf));

	buf[nrd] = 0;

	//std::cerr << "buf: " << buf << std::endl;
	//std::cerr << "len: " << len << std::endl;
	std::vector<std::string> ret = split(buf, "&");

	std::string result;

	for (auto &s : ret) {
		std::vector<std::string> v = split(s, "=");
		if (v.size() == 2) {
			result += v[0] + ": " + v[1] + "\n";
		}
	}

	// 向管道中写入
	write(1, result.c_str(), result.size());

	return 0;
}
