cmake_minimum_required(VERSION 3.12)
project(SHTTPD C)

set(CMAKE_C_STANDARD 11)

add_executable(SHTTPD main.c config/config.h config/config.c)