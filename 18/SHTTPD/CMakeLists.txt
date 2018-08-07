cmake_minimum_required(VERSION 3.12)
project(SHTTPD C)

set(CMAKE_C_STANDARD 11)

add_executable(SHTTPD main.c config/config.h config/config.c worker/worker.c worker/worker.h uri/uri.c uri/uri.h method/method.h method/method.c base/base.h)