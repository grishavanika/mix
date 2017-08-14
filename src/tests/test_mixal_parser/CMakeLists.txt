cmake_minimum_required(VERSION 3.4)

set(exe_name test_mixal_parser)

file(GLOB_RECURSE ${exe_name}_src "source" "*.cpp")
file(GLOB_RECURSE ${exe_name}_hds "source" "*.h")

add_executable(${exe_name} ${${exe_name}_src} ${${exe_name}_hds})
target_include_directories(${exe_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${exe_name} PRIVATE mixal_parser_lib)
target_link_libraries(${exe_name} PRIVATE gtest gmock gmock_main)

install(TARGETS ${exe_name}
	DESTINATION bin/tests/Debug
	CONFIGURATIONS Debug)

install(TARGETS ${exe_name}
	DESTINATION bin/tests/Release
	CONFIGURATIONS Release)
