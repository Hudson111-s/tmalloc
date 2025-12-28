cmake_minimum_required(VERSION 3.10)
project(tmalloc C)

add_library(tmalloc STATIC
    src/tmalloc.c
    src/tmalloc_heap.c
    src/tmalloc_time.h
    src/tmalloc_thread.c
)

target_include_directories(tmalloc PUBLIC include)
