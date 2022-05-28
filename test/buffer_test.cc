//
// Created by 26372 on 2022/5/22.
//

#include <iostream>
#include "utils.h"
#include "buffer.h"

void test1() {

	std::string str = "abcde\nfghi\rjklmn\r\naaa\n";

	std::string line;
	while (read_line(str, line)) {
		std::cout << line << std::endl;

	}
}

void test2() {
	Buffer b(20);
	const char *res = "hello world, helloworldhello world, helloworldhello world, helloworldhello world, helloworldhello world, helloworldhello world, helloworld";
	b.append(res, strlen(res));

	std::cout << "size = " << b.size() << std::endl;
	std::cout << "capa = " << b.capacity() << std::endl;

	b.append(res, strlen(res));
	std::cout << "size = " << b.size() << std::endl;
	std::cout << "capa = " << b.capacity() << std::endl;

	b.seek(10, Buffer::SET_OFFSET);
	std::cout << b.rd_ptr() << std::endl;
	b.seek(10, Buffer::SET_CURT);
	std::cout << b.rd_ptr() << std::endl;
}


int main() {
	//test1();
	test2();
}