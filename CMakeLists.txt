
cmake_minimum_required(VERSION 3.10)

project(matrix_multiplication_lab LANGUAGES C)

# C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Executable
add_executable(matmult
        src/main.c
        src/matrix.c
        src/sequential.c
        src/parallel.c
)

# Include directories
target_include_directories(matmult PRIVATE
        include
)

# POSIX configuration
target_compile_definitions(matmult PRIVATE
        _POSIX_C_SOURCE=199309L
)

# Compiler options
target_compile_options(matmult PRIVATE
        -O3
        -Wall
        -Wextra
)

# Thread support
find_package(Threads REQUIRED)

target_link_libraries(matmult PRIVATE
        Threads::Threads
        m
)
