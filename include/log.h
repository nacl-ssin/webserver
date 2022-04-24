//
// Created by 26372 on 2022/4/17.
//

#ifndef WEBSERVER_LOG_H
#define WEBSERVER_LOG_H

#include <string>
#include "utils.h"

enum class Level : char {
    INFO,
    WARRING,
    ERROR,
    FATAL
};


#define LOG_INFO(format, ...) log(format, level_str(Level::INFO), __FILE__, __LINE__, ##__VA_ARGS__)

#define LOG_WARRING(format, ...) log(format, level_str(Level::WARRING), __FILE__, __LINE__, ##__VA_ARGS__)

#define LOG_ERROR(format, ...) log(format, level_str(Level::ERROR), __FILE__, __LINE__, ##__VA_ARGS__)

#define LOG_FATAL(format, ...) log(format, level_str(Level::FATAL), __FILE__, __LINE__, ##__VA_ARGS__)


void log(const std::string& format, const std::string& level, const std::string& file, int line, ...);


const char *level_str(Level level);


#endif //WEBSERVER_LOG_H
