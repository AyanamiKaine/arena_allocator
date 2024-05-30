cmake_minimum_required(VERSION  3.10)
project(ARENA_ALLOCATOR)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Include directory
include_directories(include)

# Library source files
add_library(ARENA_ALLOCATOR STATIC src/arena.c)  # or SHARED for a shared library

# Set target properties (optional but recommended)
set_target_properties(ARENA_ALLOCATOR PROPERTIES
    VERSION 1.0.0
    SOVERSION 1
    PUBLIC_HEADER include/arena.h
)

# Install the library and header file
install(TARGETS ARENA_ALLOCATOR
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        PUBLIC_HEADER DESTINATION include)