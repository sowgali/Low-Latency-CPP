cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_LANGUAGE_COMPILER "/usr/bin/g++")
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_FLAGS "-std=c++2a -Wall -Wextra -Werror -Wpedantic")
set(CMAKE_VERBOSE_MAKEFILE on)

project(Low_Latency_Trading_Engine)

add_subdirectory(Chapter-4)