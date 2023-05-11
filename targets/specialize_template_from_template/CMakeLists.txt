cmake_minimum_required(VERSION 3.20 FATAL_ERROR)
project(specialize_template_from_template)

add_executable(${PROJECT_NAME})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_sources(${PROJECT_NAME} PRIVATE main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE $<$<PLATFORM_ID:Windows>:Dbghelp.lib>)