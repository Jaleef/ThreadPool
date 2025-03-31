cmake_minimum_required(VERSION 3.10)
project(threadpool)

set(CMAKE_CXX_STANDARD 17)

set(SOURCES
    src/main.cpp
    src/ThreadPool.cpp
    src/Thread.cpp
)

add_executable(threadpool ${SOURCES})

target_include_directories(
    threadpool
    PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)
