cmake_minimum_required(VERSION 3.16)
project(workqueue C)

set(CMAKE_C_STANDARD 90)

include_directories(BEFORE /usr/src/linux-headers-${CMAKE_HOST_SYSTEM_VERSION}/include SYSTEM)

add_executable(workqueue my_workqueue.c)
