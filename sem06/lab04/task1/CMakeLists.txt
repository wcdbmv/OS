cmake_minimum_required(VERSION 3.15)
project(task1 C)

add_compile_options(
	-Werror

	-Wall
	-Wextra
	-Wpedantic
)

set(CMAKE_C_STANDARD 11)

add_executable(task1 proc.c)
