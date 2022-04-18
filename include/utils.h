//
// Created by 26372 on 2022/4/17.
//

#include <string>
#include <stdio.h>
#include <ctime>
#include <sys/types.h>
#include <sys/socket.h>

/**
 * 获取系统当前时间
 * @return
 */
std::string cur_time();

/**
 * 通过文件描述符fd，读取一行数据到dest
 * @param fd
 * @param dest
 * @return
 */
size_t read_line(int fd, std::string &dest);
