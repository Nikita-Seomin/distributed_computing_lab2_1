cmake_minimum_required(VERSION 3.26)
project(lab2_2 C)

set(CMAKE_C_STANDARD 23)
find_package(MPI REQUIRED)
# Add MPI Package to Project
add_executable(lab2_2 main.c)


# Add libraries for code completion and compiling
target_link_libraries(lab2_2 PUBLIC MPI::MPI_C)