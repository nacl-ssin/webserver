//
// Created by 26372 on 2022/4/18.
//

#include "iocntl.h"

IOCntl::IOCntl() : epollfd_(epoll_create(1)), evs_(512) {

}

IOCntl::~IOCntl() {
    if (epollfd_ >= 0) {
        close(epollfd_);
    }
}

bool IOCntl::add_event(int fd, int events) const {
    epoll_event ev = {};
    ev.events = events;
    ev.data.fd = fd;
    return epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &ev) == 0;
}

bool IOCntl::cancel(int fd) const {
    return epoll_ctl(epollfd_, EPOLL_CTL_DEL, fd, nullptr) == 0;
}

bool IOCntl::modify(int fd, int events) const {
    epoll_event ev = {};
    ev.events = events;
    ev.data.fd = fd;
    return epoll_ctl(epollfd_, EPOLL_CTL_MOD, fd, &ev) == 0;
}

int IOCntl::wait(int timeout) {
    return epoll_wait(epollfd_, &evs_[0], static_cast<int>(evs_.size()), timeout);
}

epoll_event *IOCntl::get_events() {
    return &evs_[0];
}



