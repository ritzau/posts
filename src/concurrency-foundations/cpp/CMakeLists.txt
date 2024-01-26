cmake_minimum_required(VERSION 3.28)
project(concurrency-foundations CXX)
set(CMAKE_CXX_STANDARD 20)

add_executable(async async.cpp)
add_executable(data_race data_race.cpp)
add_executable(sum_example sum.cpp)
add_executable(threads threads.cpp)
# add_executable(ranges ranges.cpp)
