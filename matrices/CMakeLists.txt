cmake_minimum_required(VERSION 3.16)
project(matrices)

set(CMAKE_CXX_STANDARD 20)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)



add_executable(matrices main.cpp matrix.cpp matrix.h)
target_link_libraries(matrices Threads::Threads)