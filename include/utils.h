//
// Created by 26372 on 2022/4/17.
//

#include <string>
#include <cstdio>
#include <ctime>
#include <vector>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unordered_map>



/**
 * 获取系统时间
 * @return
 */
std::string cur_time();


/**
 * 通过src读取一行数据返回读到的字符串和字符串的结尾位置
 * @param fd
 * @param dest
 * @param start
 * @return
 */
std::pair<std::string, size_t> read_line(const std::string &src, size_t start);


/**
 * 读取一行数据，如果有一行，返回true
 * @param src
 * @param line
 * @return
 */
bool read_line(std::string &src, std::string &line);


/**
 * 字符串切分
 * @return
 */
std::vector<std::string> split(const std::string &src, const std::string &rep);



/**
 * 将字符串变为大写
 * @param str
 */
std::string to_upper(const std::string &str);


/**
 * 忽略大小写比较两个字符串
 * @param str1
 * @param str2
 * @return
 */
bool strcmpi(const std::string &str1, const std::string &str2);


/**
 * 设置非阻塞
 * @param fd
 * @return
 */
bool set_nonblock(int fd);