#include <iostream>
#include "webserver.h"

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

int main() {
	Webserver svr(8080);
	svr.dispatch();

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

	return 0;
}
