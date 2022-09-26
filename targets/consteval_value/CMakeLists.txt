cmake_minimum_required(VERSION 3.20 FATAL_ERROR)
project(consteval_value)

add_executable(${PROJECT_NAME})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
target_sources(${PROJECT_NAME} PRIVATE main.cpp)
