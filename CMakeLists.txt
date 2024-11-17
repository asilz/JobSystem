cmake_minimum_required(VERSION 3.10)
project(JobSystem)

set(CMAKE_BUILD_TYPE Debug)
set (CMAKE_CXX_STANDARD 23)

#set(CMAKE_CXX_FLAGS_DEBUG "-g")
#set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(SOURCES
    src/main.cpp
)

add_executable(devtool ${SOURCES})
target_compile_options(devtool PRIVATE -Wall -Werror=vla)
target_include_directories(devtool PRIVATE include)