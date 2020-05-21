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
)

set(
	HEADERS
	color.h
)

add_executable(lab02 main.c ${SOURCES} ${HEADERS})
