cmake_minimum_required(VERSION 4.2)
project(untitled C)

set(CMAKE_C_STANDARD 11)

add_executable(untitled main.c
        encoder.h
        encoder.c
        decoder.h
        decoder.c
        executor.h
        executor.c
)
