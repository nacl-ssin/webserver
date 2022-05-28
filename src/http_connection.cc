//
// Created by 26372 on 2022/4/17.
//

#include "http_connection.h"


uint32_t HttpConnection::_trigger_mode = EPOLLET;

std::string HttpConnection::_static_resource_root_path = "./wwwroot";
//std::string HttpConnection::_static_resource_root_path = "../wwwroot";


HttpConnection::HttpConnection() : _fd(-1), _closed(false), _addr() {
}


HttpConnection::HttpConnection(int sock, sockaddr_in addr) : _fd(sock), _closed(false), _addr(addr) {
}


std::string HttpConnection::server_name() {
	std::string ip(16, 0);
	inet_ntop(AF_INET, &_addr.sin_addr.s_addr, &ip[0], 16);
	return ip;
}


int HttpConnection::receive() {
	char buf[512];
	do {
		memset(buf, 0, sizeof(buf));
		//LOG_INFO("read begin");
		ssize_t nread = read(_fd, buf, sizeof(buf) - 1);
		//LOG_INFO("read = %s, nread = %d", buf, nread);

		if (nread > 0) {
			_inbuffer.append(buf);
			_request.parse(_inbuffer);
		} else if (nread == 0) {
			return 0;
		} else {
			// 底层没有数据了
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				return 0;
			}

			// 被信号中断
			if (errno == EINTR) {
				continue;
			}

			// 出错
			return -1;
		}
		//sleep(1);
	} while (HttpConnection::is_et());
}


int HttpConnection::send_file() {
	// 获取请求资源的路径
	std::string fpath = get_resource_full_path();
	if (access(fpath.c_str(), F_OK) != 0) {
		return send_error();
	}
	// 文件描述符
	int fd = -1;
	// 获取文件属性
	struct stat st = {};
	int ret = stat(fpath.c_str(), &st);

	if (ret == -1) {
		LOG_ERROR("stat error, path = %s, cause: %s", fpath.c_str(), strerror(errno));
		_response.set_code(404)
				.set_body(_request._path + " Not Found!");
	} else {
		// 这是一个目录 or 路径不存在都是Not Found
		if (S_ISDIR(st.st_mode)) {
			// 默认访问index.html
			_request._path = "/index.html";
			// 获取主页文件属性
			fpath = get_resource_full_path();
			if (stat(fpath.c_str(), &st) != -1) {
				goto RD_FILE;
			}
			LOG_ERROR("stat error, path = %s, cause: %s", fpath.c_str(), strerror(errno));
			_response.set_code(404)
					.set_body(_request._path + " Not Found!");
		} else if ((st.st_mode & S_IXUSR) || (st.st_mode & S_IXGRP) || (st.st_mode & S_IXOTH)) {
			// 是一个可执行文件，特殊处理
			cgi_handler();
		} else {
			// 是一个资源文件，打开文件准备响应正文
			RD_FILE:
			fd = open(fpath.c_str(), O_RDONLY);
			if (fd >= 0) {
				_response.set_code(200)
						.set_header("Content-Type", file_type(_request._path.substr(_request._path.rfind('.') + 1)))
						.set_header("Content-Length", st.st_size);
			} else {
				// 文件打开失败
				LOG_INFO("open [%s] error!, cause: %s", fpath.c_str(), strerror(errno));
				_response.set_code(500)
						.set_body("Server Failed!");
			}
		}
	}

	// 请求行、请求头、请求空行
	send();

	//发送响应行、报头、空行，如果有响应体的话，一起发送
	// 文件打开成功！
	// 改接口可以直接将文件的数据直接拷贝到网卡缓冲区中，就不用先拷贝到用户缓冲区，然后再拷贝至网卡缓冲区，少了一次拷贝
	if (fd >= 0) {
		ssize_t n = st.st_size;
		ssize_t write_size = 0;
		do {
			off_t of = write_size;
			ssize_t nsend = sendfile(_fd, fd, &of, n);
			if (nsend > 0) {
				n -= nsend;
				write_size += nsend;
			} else {
				if (errno == EAGAIN || errno == EWOULDBLOCK) {
					char buf[n];
					memset(buf, 0, n);
					lseek(fd, write_size, SEEK_SET);
					read(fd, buf, n);

					LOG_WARRING("send = %d", write_size);
					LOG_WARRING("no send = %d", n);

					_outbuffer.append(buf, n);
					break;
				}

				if (errno == EINTR) {
					continue;
				}

				::close(fd);
				return -1;
			}
		} while (HttpConnection::is_et() && n > 0);
		::close(fd);
		LOG_INFO("last");
		return 0;
	}
	return 0;
}


int HttpConnection::send() {
	std::string res = _response.build();
	size_t n = res.size();
	ssize_t write_size = 0;

	do {
		ssize_t nwt = write(_fd, res.c_str() + write_size, n);
		if (nwt > 0) {
			n -= nwt;
			write_size += nwt;
		} else {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				//_outbuffer = res.substr(write_size);
				_outbuffer.append(res.c_str() + write_size, n);
				break;
			}

			if (errno == EINTR) {
				continue;
			}

			//close();
			return -1;
		}
	} while (HttpConnection::is_et() && n > 0);
	_response._header.clear();
	_response._body.clear();
	return 0;
}


int HttpConnection::send_buffer() {
	size_t n = _outbuffer.size();
	size_t write_size = 0;
	LOG_WARRING("send buffer fd = %d, size = %d", _fd, n);

	do {
		ssize_t nwt = write(_fd, _outbuffer.rd_ptr(), n);
		if (nwt > 0) {
			n -= nwt;
			write_size += nwt;
			_outbuffer.seek(nwt, Buffer::SET_CURT);
			LOG_WARRING("send = %d", write_size);
			LOG_WARRING("no send = %d", n);
			if (n <= 0) {
				_outbuffer.clear();
				break;
			}
		} else {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				//_outbuffer.seek(write_size, Buffer::SET_CURT);
				LOG_WARRING("nwt = %d", nwt);
				LOG_WARRING("send = %d", write_size);
				LOG_WARRING("no send = %d", _outbuffer.size());
				return 0;
			}

			if (errno == EINTR) {
				continue;
			}

			//close();
			return -1;
		}
	} while (HttpConnection::is_et());
}


int HttpConnection::send_error() {
	_response.set_code(404);
	_response.set_body(_request._path + " Not Found");
	return send();
}


bool HttpConnection::keep_alive() {
	for (auto &p : _request._header) {
		if (strcmpi("Connection", p.first) != 0 && strcmpi(p.second, "keep-alive")) {
			return true;
		}
	}
	return false;
}


void HttpConnection::cgi_handler() {
	// 可执行程序路径
	std::string exec_path = _static_resource_root_path + _request._path;
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
		::close(in[1]);
		::close(out[0]);

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
		::close(in[0]);
		::close(out[1]);

		// 向子进程中写入数据，也就是写入管道

		LOG_INFO("wait proc quit");
		std::size_t pos = 0;
		while (pos < params.size()) {
			ssize_t nwr = write(in[1], params.c_str() + pos, params.size());
			if (nwr > 0) {
				pos += nwr;
			} else {
				LOG_ERROR("send_file to proc error");
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
				_response.set_code(200)
						.set_header("Content-Length", _response._body.size());
			} else {
				LOG_INFO("proc failed quit!");
				_response.set_code(500)
						.set_body("Server Failed!");
			}
		} else {
			LOG_INFO("proc get a sig!");
			_response.set_code(500)
					.set_body("Server Failed!");
		}
	} else {
		// error
		LOG_ERROR("fork error, cause: %s", strerror(errno));
	}
}












