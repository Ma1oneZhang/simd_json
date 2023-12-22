#
# Created by ziyang on 12/17/23.
#
include_directories(../src)

add_executable(test test.cpp)
target_link_libraries(test PRIVATE gtest gtest_main simd_json_shared)