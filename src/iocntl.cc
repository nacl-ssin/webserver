//
// Created by 26372 on 2022/4/18.
//

#include "iocntl.h"

IOCntl::IOCntl() : _epollfd(epoll_create(1)), _evs(512) {

}

IOCntl::~IOCntl() {
    if (_epollfd >= 0) {
        close(_epollfd);
    }
}

bool IOCntl::add_event(int fd, int events) const {
    epoll_event ev = {};
    ev.events = events;
    ev.data.fd = fd;
    return epoll_ctl(_epollfd, EPOLL_CTL_ADD, fd, &ev) == 0;
}

bool IOCntl::cancel(int fd) const {
    return epoll_ctl(_epollfd, EPOLL_CTL_DEL, fd, nullptr) == 0;
}

bool IOCntl::modify(int fd, int events) const {
    epoll_event ev = {};
    ev.events = events;
    ev.data.fd = fd;
    return epoll_ctl(_epollfd, EPOLL_CTL_MOD, fd, &ev) == 0;
}

int IOCntl::wait(int timeout) {
    return epoll_wait(_epollfd, &_evs[0], static_cast<int>(_evs.size()), timeout);
}

epoll_event *IOCntl::get_events() {
    return &_evs[0];
}



