cmake_minimum_required(VERSION 3.15)
project(task2 C)

set(CMAKE_C_STANDARD 11)

include_directories(BEFORE /usr/src/linux-headers-5.3.0-46/include SYSTEM)

add_executable(task2 fortune.c)
