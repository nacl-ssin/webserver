//
// Created by 26372 on 2022/4/17.
//

#include "http_connection.h"


//const std::string HttpConnection::RESOURCE_ROOT = "../static";
const std::string HttpConnection::RESOURCE_ROOT = "/home/nacl/remote_proj/clion/webserver/static";
//const std::string HttpConnection::RESOURCE_ROOT = "/root/webserver/static";



HttpConnection::HttpConnection() : _fd(-1), _addr() {

}


HttpConnection::HttpConnection(int sock, sockaddr_in addr) : _fd(sock), _addr(addr) {

}


std::string HttpConnection::server_name() {
	std::string ip(16, 0);
	inet_ntop(AF_INET, &_addr.sin_addr.s_addr, &ip[0], 16);
	return ip;
}


ssize_t HttpConnection::receive() {
	char buf[4096] = {0};
	memset(buf, 0, sizeof(buf));
	ssize_t nread = recv(_fd, buf, sizeof(buf) - 1, 0);
	if (nread < 0) {
		LOG_ERROR("receive error fd = %d", _fd);
		return nread;
	}

	if (nread == 0) {
		return nread;
	}
	buf[nread] = 0;
	_request.parse(buf);
	return nread;
}


void HttpConnection::send() {
	_response._version = _request._version;
	//LOG_INFO("request path = %s", _request._path.c_str());

	// 是否打开了文件
	bool is_open = false;
	// 文件描述符
	int fd = -1;

	// 获取请求资源的路径
	std::string path = RESOURCE_ROOT + _request._path;
	// 获取文件属性
	struct stat st = {};
	int ret = stat(path.c_str(), &st);

	if (ret == -1 || S_ISDIR(st.st_mode)) {
		// 这是一个目录 or 路径不存在都是Not Found
		if (_request._path == "/") {
			// 默认访问index.html
			_request._path = "/index.html";
			// 获取主页文件属性
			stat((RESOURCE_ROOT + _request._path).c_str(), &st);
			goto RD_FILE;
		}
		LOG_ERROR("stat error, cause: %s", strerror(errno));
		_response.set_code(404);
		_response.set_body(_request._path + " Not Found!");
	} else {
		// 检测是什么资源
		if ((st.st_mode & S_IXUSR) || (st.st_mode & S_IXGRP) || (st.st_mode & S_IXOTH)) {
			// 是一个可执行文件，特殊处理
			cgi_handler();
		} else {
			// 是一个文件，打开文件准备响应正文
			RD_FILE:
			fd = open((RESOURCE_ROOT + _request._path).c_str(), O_RDONLY);
			if (fd >= 0) {
				_response.set_code(200);
				//_response._header.emplace_back("Content-Type", file_type(_request._path.substr(_request._path.rfind('.') + 1)));
				//_response._header.emplace_back("Content-Length", std::to_string(st.st_size));
				_response.set_header("Content-Type", file_type(_request._path.substr(_request._path.rfind('.') + 1)));
				_response.set_header("Content-Length", st.st_size);
				is_open = true;
			} else {
				// 文件打开失败
				LOG_INFO("open [%s] error!, cause: %s", (RESOURCE_ROOT + _request._path).c_str(), strerror(errno));
				_response.set_code(500);
				_response.set_body("Server Failed!");
			}
		}
	}

	std::string res = _response.build();
	 //发送响应行、报头、空行
	::send(_fd, res.c_str(), res.size(), 0);
	//LOG_INFO("send ok");
	//int n = ::send(_fd, "HTTP/1.0 200 OK\nContent-Legnth: 2\n\nOK", 37, 0);
	//LOG_INFO("send n = %d", n);
	//
	//LOG_INFO("send end");
	//LOG_INFO("response = %s", res.c_str());

	// 文件打开成功！
	// 改接口可以直接将文件的数据直接拷贝到网卡缓冲区中，就不用先拷贝到用户缓冲区，然后再拷贝至网卡缓冲区，少了一次拷贝
	if (is_open) {
		sendfile(_fd, fd, nullptr, st.st_size);
		close(fd);
	}

	_response._header.clear();
	_response._body.clear();
}


void HttpConnection::cgi_handler() {
	// 可执行程序路径
	std::string exec_path = RESOURCE_ROOT + _request._path;
	LOG_INFO("cgi proc path: %s", exec_path.c_str());

	// 创建管道用于父子进程通信，程序替换不会替换内核中的数据结构，比如文件描述符数据
	int in[2] = {0};
	int out[2] = {0};

	if (pipe(in) != 0) {
		LOG_ERROR("pipe error, cause: %s", strerror(errno));
	}
	if (pipe(out) != 0) {
		LOG_ERROR("pipe error, cause: %s", strerror(errno));
	}

	// 将要向管道写入的参数
	std::string params;
	if (_request._method == "POST") {
		params = _request._body;
	} else {
		params = _request._query;
	}

	// 通过创建子进程来执行cgi程序
	pid_t pid = fork();

	if (pid == 0) {
		// 子进程
		close(in[1]);
		close(out[0]);

		std::string len = "CONTENT_LEN=" + std::to_string(params.size());
		putenv(const_cast<char *>(len.c_str()));

		// 使用dup2将子进程的输入输出，重定向到管道
		dup2(in[0], 0);
		dup2(out[1], 1);


		// 使用程序替换执行cgi程序
		execl(exec_path.c_str(), _request._path.c_str(), nullptr);

		LOG_ERROR("execl error, cause: %s", strerror(errno));
		exit(0);
	} else if (pid > 0) {
		// 父进程
		close(in[0]);
		close(out[1]);

		// 向子进程中写入数据，也就是写入管道

		LOG_INFO("wait proc quit");
		std::size_t pos = 0;
		while (pos < params.size()) {
			ssize_t nwr = write(in[1], params.c_str() + pos, params.size());
			if (nwr > 0) {
				pos += nwr;
			} else {
				LOG_ERROR("send to proc error");
				break;
			}
		}

		// 从子进程中读取数据

		char buf[128] = {0};
		while (true) {
			ssize_t nrd = read(out[0], buf, sizeof(buf) - 1);
			LOG_INFO("nrd = %d", nrd);
			if (nrd <= 0) {
				break;
			}
			_response._body += buf;
		}

		// 等待子进程退出
		LOG_INFO("wait proc quit");
		int status = 0;
		waitpid(pid, &status, WNOWAIT);

		// 检测子进程是否正常执行完毕
		if (WIFEXITED(status)) {
			if (WEXITSTATUS(status) == 0) {
				LOG_INFO("proc normal quit!");
				_response.set_code(200);
				_response.set_header("Content-Length", _response._body.size());
			} else {
				LOG_INFO("proc failed quit!");
				_response.set_code(500);
				_response.set_body("Server Failed!");
			}
		} else {
			LOG_INFO("proc get a sig!");
			_response.set_code(500);
			_response.set_body("Server Failed!");
		}
	} else {
		// error
		LOG_ERROR("fork error, cause: %s", strerror(errno));
	}
}







