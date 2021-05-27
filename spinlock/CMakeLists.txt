cmake_minimum_required(VERSION 3.16)
project(spinlock)

set(CMAKE_CXX_STANDARD 20)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(spinlock main.cpp)
target_link_libraries(spinlock Threads::Threads)