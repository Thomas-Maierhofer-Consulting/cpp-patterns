cmake_minimum_required(VERSION 3.20 FATAL_ERROR)
project(cpp_patterns)


if (MSVC)
    # warning level 4 and all warnings as errors
    add_compile_options(/W4)
else()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -Wextra -pedantic)
endif()

add_subdirectory(src/consteval_value)
add_subdirectory(src/copy_swap_idiom)
add_subdirectory(src/object_layout_deduction)
add_subdirectory(src/type_literals)