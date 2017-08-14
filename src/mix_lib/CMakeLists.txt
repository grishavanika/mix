cmake_minimum_required(VERSION 3.2)

set(lib_name mix_lib)

file(GLOB_RECURSE ${lib_name}_src "source" "*.cpp")
file(GLOB_RECURSE ${lib_name}_hds "include" "*.h")

# See internal/valarray_register_helpers.h std::copy() call.
# Can be removed once valarray_register_helpers.h will be removed
add_definitions(-D_SCL_SECURE_NO_WARNINGS)
add_library(${lib_name} STATIC ${${lib_name}_src} ${${lib_name}_hds})
target_link_libraries(${lib_name} PUBLIC core_lib)

target_include_directories(${lib_name} PUBLIC include)

