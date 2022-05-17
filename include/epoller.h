//
// Created by 26372 on 2022/4/18.
//

#ifndef WEBSERVER_EPOLLER_H
#define WEBSERVER_EPOLLER_H

#include <epoller.h>
#include <vector>
#include <unistd.h>
#include <sys/epoll.h>

class Epoller {
private:
    int _epollfd;
    std::vector<epoll_event> _evs;

public:
    Epoller(int evs_size = 128);

    ~Epoller();

    /**
     * 添加io检测
     * @param ei
     * @return
     */
    bool add_event(int fd, int events) const;

    /**
     * 取消io检测
     * @param fd
     * @return
     */
    bool cancel(int fd) const;

    /**
     * 修改io检测
     * @param ei
     * @return
     */
    bool modify(int fd, int events) const;

    /**
     * io事件等待
     * @param timeout
     * @return
     */
    int wait(int timeout);

    /**
     * 获取可操作io
     * @return
     */
    epoll_event *get_events();
};


#endif //WEBSERVER_EPOLLER_H
