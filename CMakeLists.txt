cmake_minimum_required(VERSION 3.8)
project(Lab4)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp Factorizer.h Factorizer.cpp Threader.h)
add_executable(Lab4 ${SOURCE_FILES})