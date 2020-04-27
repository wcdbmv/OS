cmake_minimum_required(VERSION 3.15)
project(lab08 C)

set(CMAKE_C_STANDARD 90)

include_directories(BEFORE /usr/src/linux-headers-5.3.0-46/include SYSTEM)

add_executable(lab08 myfs.c)
