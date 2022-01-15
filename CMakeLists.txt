cmake_minimum_required(VERSION 3.5)

project(threadpool LANGUAGES CXX)
add_definitions(-Ddebuglog)
set(CMAKE_VERBOSE_MAKEFILEON ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 17)
# 声明一个 cmake 工程
set(LINK_DIR  /usr/lib /usr/local/lib  /usr/lib/x86_64-linux-gnu/)
INCLUDE_DIRECTORIES(./)
link_directories(${LINK_DIR})
add_executable(threadpool main.cpp)
target_link_libraries(threadpool libpthread.so)

