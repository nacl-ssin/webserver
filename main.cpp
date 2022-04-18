#include <iostream>
#include "webserver.h"
#include "log.h"

int main() {
    LOG_INFO("hello world");
    LOG_WARRING("hello world");
    LOG_ERROR("hello world");
    LOG_FATAL("hello world");
    return 0;
}
