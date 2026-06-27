cmake_minimum_required(VERSION 3.22)
project(spc)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES "src/*.c")

add_executable(server ${SOURCES})

target_include_directories(server PRIVATE
    include
    lib
)
