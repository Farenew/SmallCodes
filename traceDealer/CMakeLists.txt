cmake_minimum_required(VERSION 3.5)
project(traceDealer)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp trace.cpp trace.h)
add_executable(traceDealer ${SOURCE_FILES})