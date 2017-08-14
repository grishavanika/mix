cmake_minimum_required(VERSION 3.2)

set(lib_name mixal_formatter_lib)

file(GLOB_RECURSE ${lib_name}_src "source" "*.cpp")
file(GLOB_RECURSE ${lib_name}_hds "include" "*.h")

add_library(${lib_name} STATIC ${${lib_name}_src} ${${lib_name}_hds})
target_link_libraries(${lib_name} PUBLIC mixal_parser_lib)

target_include_directories(${lib_name} PUBLIC include)
