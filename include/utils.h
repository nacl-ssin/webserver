//
// Created by 26372 on 2022/4/17.
//

#include <string>
#include <cstdio>
#include <ctime>
#include <vector>
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
 * 字符串切分
 * @return
 */
std::vector<std::string> split(const std::string &src, const std::string &rep);



/**
 * 根据文件后缀获取content-type
 * @param suffix
 * @return
 */
std::string file_type(const std::string &suffix);
