//
// Created by 26372 on 2022/4/21.
//
#include <iostream>
#include <vector>
#include <sstream>
#include <sys/types.h>
#include <sys/fcntl.h>
#include "utils.h"

class A {
public:
	int a;
	char b;
	int *c;

	A() {

	}

	void show() {
		std::cout << "A::show()" << std::endl;
	}
private:
	int d;
	char f;
	int *g;
};

void test() {
	//sockaddr_in ad;
	//
	//sockaddr_in addr = {};
	//
	//int arr[10] = {0};
	//int arr2[10] = {};
	//int arr1[10];
	//
	//A a = {};
	//A a1;

	//std::unordered_map<int, A> m;
	//A a;
	//m.insert({1, a});
	//
	//m[1].show();
}

void test2() {
	std::vector<std::string> res = split("/a/b/c?", "?");
	for (auto &e : res) {
		std::cout << e << std::endl;
	}
}

void test3() {
	int fd = open("../static/index.html", O_RDONLY);
	std::cout << "fd = " << fd << std::endl;
}

void test4() {
	std::stringstream ss;
	ss << " aaa" << " bbb";
	std::cout << ss.str() << std::endl;
}

void test5() {
	int fd = open("/", O_RDONLY);
	std::cout << "fd = " << fd << std::endl;
}

void test6() {
	std::vector<std::string> vect;
	std::string str = "ABC";
	//vect.emplace_back(std::move(str));
	vect.push_back(std::move(str));

	std::cout << "str = " << str << std::endl;
	
}

int main() {
	//test3();
	// test4();
	// test5();
	test6();
	return 0;
}

