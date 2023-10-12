cmake_minimum_required(VERSION 3.22)
project(proj2 C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(proj2
        list.c
        list.h
        hash.c
        hash.h
        bitmap.c
        bitmap.h
        hex_dump.c
        hex_dump.h
        main.c)
