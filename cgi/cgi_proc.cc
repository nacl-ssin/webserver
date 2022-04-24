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

	std::cerr << "buf: " << buf << std::endl;
	std::cerr << "len: " << len << std::endl;
	std::vector<std::string> ret = split(buf, "&");
	std::vector<int> nums;


	
	for (auto &s : ret) {
		std::cerr << "item = " << s << std::endl;
		std::vector<std::string> v = split(s, "=");
		if (v.size() == 2) {
			std::cerr << v[0] << ", " << v[1] << std::endl;
			nums.push_back(stoi(v[1]));
		}
	}

	int sum;
	for (auto e : nums) {
		sum += e;
	}

	std::string result = "Result: " + std::to_string(sum);

	ssize_t nwr = write(1, result.c_str(), result.size());

	std::cerr << "nwr = " << nwr << std::endl;

	return 0;
}
