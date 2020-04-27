cmake_minimum_required(VERSION 3.15)
project(unix C)

set(CMAKE_C_STANDARD 11)

add_compile_options(
	-Werror

	-Wall
	-Wpedantic
	-Wextra
)

add_executable(server server.c)
add_executable(client client.c)
