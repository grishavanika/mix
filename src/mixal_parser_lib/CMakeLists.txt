cmake_minimum_required(VERSION 3.4)

set(lib_name mixal_parser_lib)

file(GLOB_RECURSE ${lib_name}_src "source" "*.cpp")
file(GLOB_RECURSE ${lib_name}_hds "include" "*.h")
file(GLOB_RECURSE ${lib_name}_hds_inl "include" "*.inl")

add_library(${lib_name} STATIC ${${lib_name}_src} ${${lib_name}_hds} ${${lib_name}_hds_inl})
target_link_libraries(${lib_name} PUBLIC core_lib)

target_include_directories(${lib_name} PUBLIC include)
