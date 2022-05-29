//
// Created by 26372 on 2022/4/18.
//

#ifndef WEBSERVER_A_H
#define WEBSERVER_A_H

#include <vector>
#include <unistd.h>
#include <sys/epoll.h>
#include <unordered_map>
#include "thread_pool.hpp"

class Epoller;


struct EventItem {
	typedef void (*callback_t)(EventItem &);

public:
	int _fd;
	void *_ptr;
	Epoller *_r;
	bool _async;
	callback_t _reader;
	callback_t _writer;
	callback_t _error;

	EventItem();

	void register_event(int fd, callback_t reader, callback_t writer, callback_t error);
};


class Epoller {
	typedef void (*after_callback_t)();

private:
	int _epfd;
	thread_pool *_tp;
	std::vector<epoll_event> _evs;
	std::unordered_map<int, EventItem> _event_items;

public:
	explicit Epoller(int evs_size = 128);

	Epoller(const Epoller &ep) = delete;

	Epoller &operator=(const Epoller &ep) = delete;

	~Epoller();

	/**
	 * 添加事件
	 * @param ei
	 * @param events
	 * @return
	 */
	bool add_event(const EventItem &ei, uint32_t events);

	/**
	 * 修改io检测
	 * @param ei
	 * @return
	 */
	bool modify(int fd, uint32_t events) const;

	/**
	 * 事件派发
	 * @param timeout
	 * @param wa
	 */
	void event_dispatch(int timeout, after_callback_t ac = nullptr);

	/**
	 * 文件描述符是否存在
	 * @param fd
	 * @return
	 */
	bool exists(int fd);


	/**
	 * 取消事件监听
	 * @param fd
	 * @return
	 */
	bool cancel(int fd);
};


#endif //WEBSERVER_A_H
