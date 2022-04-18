//
// Created by 26372 on 2022/4/17.
//

#include "utils.h"


std::string cur_time() {
    time_t t;
    time(&t);
    tm *ret = localtime(&t);
    char buf[20] = {0};
    sprintf(buf, "%d-%02d-%02d %02d:%02d:%02d", ret->tm_year + 1900, ret->tm_mon + 1, ret->tm_mday, ret->tm_hour, ret->tm_min, ret->tm_sec);
    return buf;
}


size_t read_line(int fd, std::string &dest) {
    char ch = 'E';
    while (ch != '\n') {
        ssize_t nread = recv(fd, &ch, 1, 0);
        if (nread > 0) {
            if (ch == '\r') {
                ssize_t nread = recv(fd, &ch, 1, MSG_PEEK);
            }
            dest.push_back(ch);
        } else if (nread == 0) {
            return 0;
        } else {
            return 0;
        }
    }
    return dest.size();
}