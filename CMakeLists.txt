#
# Created by ziyang on 12/17/23.
#
cmake_minimum_required(VERSION 3.27)
project(tiny_simd_json)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
message("This project only write for the X86-64 instruction collection")

add_subdirectory(deps)
add_subdirectory(src)
add_subdirectory(test)