cmake_minimum_required(VERSION 3.5)
project(traceTiny)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp traceDealer.h)
add_executable(traceTiny ${SOURCE_FILES})