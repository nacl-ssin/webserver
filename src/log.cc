//
// Created by 26372 on 2022/4/17.
//
#include <cstdarg>
#include "log.h"

void log(const std::string &format, const std::string &level, const std::string &file, int line, ...) {
	std::string str =
			cur_time() + " --- " + level + " [ " + file + ":" + std::to_string(line) + " ] : " + format + "\n";
	va_list args;
	va_start(args, str.size());
	vfprintf(stdout, str.c_str(), args);
	va_end(args);
}


const char *level_str(Level level) {
	const char *ret = "";
	switch (level) {
		case Level::INFO:
			ret = "\033[32mINFO\033[0m";
			break;
		case Level::WARRING:
			ret = "\033[33mWARRING\033[0m";
			break;
		case Level::ERROR:
			ret = "\033[31mERROR\033[0m";
			break;
		case Level::FATAL:
			ret = "\033[36mFATAL\033[0m";
			break;
	}
	return ret;
}

