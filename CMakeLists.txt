cmake_minimum_required(VERSION 3.15)
project(tmalloc
    VERSION 1.0.0
    DESCRIPTION "Timed memory allocation library"
    LANGUAGES C
)

include(CMakePackageConfigHelpers)

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

add_library(tmalloc::tmalloc ALIAS tmalloc)

target_include_directories(tmalloc PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> # When building from source
    $<INSTALL_INTERFACE:include> # After installation
)

target_compile_definitions(tmalloc PRIVATE
    _POSIX_C_SOURCE=200809L
    _GNU_SOURCE
)

target_link_libraries(tmalloc PUBLIC Threads::Threads)

target_compile_options(tmalloc PRIVATE
    $<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>: -Wall -Wextra -Werror> 
    $<$<COMPILE_LANG_AND_ID:C,MSVC>: /W4 /WX /wd4152>
)

# Install.

install(TARGETS tmalloc
    EXPORT tmallocTargets
    ARCHIVE DESTINATION lib
)

install(DIRECTORY include/
    DESTINATION include
)

install(EXPORT tmallocTargets
    FILE tmallocTargets.cmake
    NAMESPACE tmalloc::
    DESTINATION lib/cmake/tmalloc
)

configure_package_config_file(
    cmake/tmallocConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/tmallocConfig.cmake"
    INSTALL_DESTINATION lib/cmake/tmalloc
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/tmallocConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/tmallocConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/tmallocConfigVersion.cmake"
    DESTINATION lib/cmake/tmalloc
)
