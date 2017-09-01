cmake_minimum_required(VERSION 3.9.1)

set(exe_name test_core)

target_collect_sources(${exe_name})

add_executable(${exe_name} ${${exe_name}_files})

target_link_libraries(${exe_name} PRIVATE core_lib)
target_link_libraries(${exe_name} PRIVATE gtest gmock gmock_main)

target_install_binaries(${exe_name})
