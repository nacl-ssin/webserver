cmake_minimum_required(VERSION 3.17)
project(webserver)

set(CMAKE_CXX_STANDARD 14)

aux_source_directory(src/ SOURCE_FILES)

include_directories(include)

add_executable(webserver main.cc ${SOURCE_FILES})

add_executable(test test/buffer_test.cc ${SOURCE_FILES})

# 链接pthread库
target_link_libraries(webserver pthread)

target_link_libraries(webserver mysqlclient)
