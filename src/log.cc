//
// Created by 26372 on 2022/4/17.
//
#include <cstdarg>
#include "log.h"

void log(const std::string format, const std::string level, const std::string file, int line, ...) {
    std::string str = cur_time() + " --- " + level + " [ " + file + ":" + std::to_string(line) + " ] : " + format + "\n";
    va_list args;
    va_start(args, str.size());
    vfprintf(stdout, str.c_str(), args);
    va_end(args);
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

