cmake_minimum_required(VERSION 3.2)

set(lib_name core_lib)

file(GLOB_RECURSE ${lib_name}_src "source" "*.cpp")
file(GLOB_RECURSE ${lib_name}_hds "include" "*.h")

add_library(${lib_name} STATIC ${${lib_name}_src} ${${lib_name}_hds})

# From: http://stackoverflow.com/questions/11216408/cmake-dependencies-headers-between-apps-libraries-in-same-project
target_include_directories(${lib_name} PUBLIC include)

