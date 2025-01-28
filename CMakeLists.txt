cmake_minimum_required(VERSION 3.13)

project(static_queue C ASM)

add_library(static_queue INTERFACE)

target_sources(static_queue INTERFACE
	src/static_queue.c
)

target_include_directories(static_queue INTERFACE
	src
)

# Option to build standalone executable for testing
option(STATIC_QUEUE_TEST "Build standalone executable for static_queue" OFF)

if(STATIC_QUEUE_TEST)
    set(CMAKE_C_COMPILER gcc)

    # Add standalone executable for testing static_queue
    add_executable(test_static_queue test/test_static_queue.c)

    # Link the static_queue library to the standalone executable
    target_link_libraries(test_static_queue PRIVATE static_queue)

    # Optionally, add any specific compiler options for testing
    target_compile_options(test_static_queue PRIVATE -Wall -Wextra -pedantic)
endif()