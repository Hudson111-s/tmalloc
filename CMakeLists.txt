cmake_minimum_required(VERSION 3.10)
project(tmalloc C)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_library(tmalloc STATIC
    src/tmalloc.c
    src/tmalloc_heap.c
    src/tmalloc_thread.c
)

target_include_directories(tmalloc PUBLIC include)

target_link_libraries(tmalloc PUBLIC Threads::Threads)

target_compile_options(tmalloc PRIVATE
    $<$<OR:$<C_COMPILER_ID:GNU>,$<C_COMPILER_ID:Clang>,$<C_COMPILER_ID:AppleClang>>:
        -Wall -Wextra
    >
    $<$<C_COMPILER_ID:MSVC>:
        /W4 /WX
        /wd4152
    >
)

set_target_properties(tmalloc PROPERTIES
    C_STANDARD 11
    C_STANDARD_REQUIRED ON
    C_EXTENSIONS OFF
)
