cmake_minimum_required(VERSION 3.9.1)
set(exe_name test_mix)

target_collect_sources(${exe_name})

add_executable(${exe_name} ${${exe_name}_files})
target_include_directories(${exe_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

set_target_precompiled_header(${exe_name} precompiled.h precompiled.cpp)

target_link_libraries(${exe_name} PRIVATE mix_lib)
target_link_libraries(${exe_name} PRIVATE gtest gmock gmock_main)

target_install_binaries(${exe_name})

