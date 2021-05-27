cmake_minimum_required(VERSION 3.16)
project(Stack)

set(CMAKE_CXX_STANDARD 20)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(Stack main.cpp)
target_link_libraries(Stack Threads::Threads)