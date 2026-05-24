cmake_minimum_required(VERSION 3.15)
project(tmalloc C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_library(tmalloc STATIC
    src/tmalloc.c
    src/tmalloc_heap.c
    src/tmalloc_thread.c
)

target_include_directories(tmalloc PUBLIC include)
target_compile_definitions(tmalloc PRIVATE
    _POSIX_C_SOURCE=200809L
    _GNU_SOURCE
)

target_link_libraries(tmalloc PUBLIC Threads::Threads)

target_compile_options(tmalloc PRIVATE
    $<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>: -Wall -Wextra > 
    $<$<COMPILE_LANG_AND_ID:C,MSVC>: /W4 /WX /wd4152 >
)
