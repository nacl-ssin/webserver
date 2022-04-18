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

#define LOG_INFO(message) log(Level::INFO, message)

#define LOG_WARRING(message) log(Level::WARRING, message)

#define LOG_ERROR(message) log(Level::ERROR, message)

#define LOG_FATAL(message) log(Level::FATAL, message)

void log(Level level, std::string message);

const char *level_str(Level level);


#endif //WEBSERVER_LOG_H
