cmake_minimum_required(VERSION 3.15)
project(lab05 C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY /home/user/bmstu/OS/sem06/lab05)

add_executable(testCIO testCIO.c)
add_executable(testKernelIO testKernelIO.c)
add_executable(testCO testCO.c)
