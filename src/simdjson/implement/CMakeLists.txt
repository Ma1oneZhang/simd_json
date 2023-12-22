#
# Created by ziyang on 12/17/23.
#
add_library(simd_json_static STATIC
        x86_simd_implement.cpp
        x86_normal_implement.cpp)

add_library(simd_json_shared SHARED
        x86_simd_implement.cpp
        x86_normal_implement.cpp
)