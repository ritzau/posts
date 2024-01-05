cmake_minimum_required(VERSION 3.28)
project(concurrency-foundations CXX)
set(CMAKE_CXX_STANDARD 20)

add_executable(threads threads.cpp)

add_executable(async async.cpp)
