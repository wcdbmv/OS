cmake_minimum_required(VERSION 3.15)
project(lab02 C)

set(CMAKE_C_STANDARD 11)

add_compile_options(
	-Werror

	-Wall
	-Wextra
	-Wpedantic
)

set(
	SOURCES
	stack.c
	string_stack.c
	tree.c
	vector.c
)

set(
	HEADERS
	color.h
	stack.h
	string_stack.h
	tree.h
	vector.h
)

add_executable(lab02 main.c ${SOURCES} ${HEADERS})
