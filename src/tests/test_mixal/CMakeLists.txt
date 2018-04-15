set(exe_name test_mixal)

target_collect_sources(${exe_name})

add_executable(${exe_name} ${${exe_name}_files})
target_include_directories(${exe_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

set_all_warnings(${exe_name} PRIVATE)

target_link_libraries(${exe_name} PRIVATE mixal_lib)
target_link_libraries(${exe_name} PRIVATE gtest gmock gmock_main)

target_install_binaries(${exe_name})

