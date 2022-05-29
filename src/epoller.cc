//
// Created by 26372 on 2022/4/18.
//

#include "epoller.h"

EventItem::EventItem() : _fd(-1), _ptr(nullptr), _r(nullptr), _async(false), _reader(nullptr), _writer(nullptr),
						 _error(nullptr) {

}


void EventItem::register_event(int fd, EventItem::callback_t reader, EventItem::callback_t writer,
							   EventItem::callback_t error) {
	_fd = fd;
	_reader = reader;
	_writer = writer;
	_error = error;
}


Epoller::Epoller(int evs_size) : _epfd(-1), _tp(new thread_pool), _evs(evs_size) {
	_epfd = epoll_create(1);
	if (_epfd < 0) {
		LOG_FATAL("epoll create failed!");
		exit(1);
	}
	_tp->start();
}


Epoller::~Epoller() {
	if (_epfd >= 0) {
		close(_epfd);
	}
}


bool Epoller::add_event(const EventItem &ei, uint32_t events) {
	epoll_event ev = {};
	ev.events = events;
	ev.data.fd = ei._fd;
	_event_items.insert({ei._fd, ei});
	return epoll_ctl(_epfd, EPOLL_CTL_ADD, ei._fd, &ev) == 0;
}


bool Epoller::exists(int fd) {
	auto iter = _event_items.find(fd);
	if (iter == _event_items.end()) {
		return false;
	}
	return true;
}


void Epoller::event_dispatch(int timeout, after_callback_t ac) {
	while (true) {
		int nready = epoll_wait(_epfd, &_evs[0], static_cast<int>(_evs.size()), timeout);

		if (ac) {
			ac();
		}

		for (int i = 0; i < nready; ++i) {
			auto e = _evs[i];
			// 出错事件
			if (e.events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) {
				if (exists(e.data.fd) && _event_items[e.data.fd]._error) {
					LOG_INFO("有出错事件了");
					_tp->push_task(_event_items[e.data.fd]._error, _event_items[e.data.fd]);
				}
			}

			// 读事件
			if (e.events & EPOLLIN) {
				if (exists(e.data.fd) && _event_items[e.data.fd]._reader) {
					LOG_INFO("有读事件了");
					if (_event_items[e.data.fd]._async) {
						_tp->push_task(_event_items[e.data.fd]._reader, _event_items[e.data.fd]);
					} else {
						_event_items[e.data.fd]._reader(_event_items[e.data.fd]);
					}
				}
			}

			// 写事件
			if (e.events & EPOLLOUT) {
				if (exists(e.data.fd) && _event_items[e.data.fd]._writer) {
					LOG_INFO("有写事件了");
					if (_event_items[e.data.fd]._async) {
						_tp->push_task(_event_items[e.data.fd]._writer, _event_items[e.data.fd]);
					} else {
						_event_items[e.data.fd]._writer(_event_items[e.data.fd]);
					}
				}
			}
		}
	}
}


bool Epoller::modify(int fd, uint32_t events) const {
	epoll_event ev = {};
	ev.events = events;
	ev.data.fd = fd;
	return epoll_ctl(_epfd, EPOLL_CTL_MOD, fd, &ev) == 0;
}


bool Epoller::cancel(int fd) {
	if (exists(fd)) {
		close(fd);
		_event_items.erase(fd);
		return epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, nullptr) == 0;
	}
	return true;
}


