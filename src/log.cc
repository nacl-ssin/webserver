//
// Created by 26372 on 2022/4/17.
//
#include "log.h"

void log(Level level, const std::string message) {
    fprintf(stdout, "[%s][%s][%s][%s:%d]\n", level_str(level), cur_time().c_str(), message.c_str(), __FILE__, __LINE__);
}

const char *level_str(Level level) {
    const char *ret = "";
    switch (level) {
        case Level::INFO:
            ret = "INFO";
            break;
        case Level::WARRING:
            ret = "WARRING";
            break;
        case Level::ERROR:
            ret = "ERROR";
            break;
        case Level::FATAL:
            ret = "FATAL";
            break;
    }
    return ret;
}
