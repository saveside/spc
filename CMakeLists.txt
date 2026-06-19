cmake_minimum_required(VERSION 4.0)
project(spc)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(server
    src/server.c
    src/commands.c
    src/utils.c
    src/rooms.c
    src/history.c
)

target_include_directories(server PRIVATE
    include
    lib
)
