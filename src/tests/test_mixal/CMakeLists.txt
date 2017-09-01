cmake_minimum_required(VERSION 3.9.1)

set(exe_name test_mixal)

file(GLOB_RECURSE ${exe_name}_src "source" "*.cpp")
file(GLOB_RECURSE ${exe_name}_hds "source" "*.h")

add_executable(${exe_name} ${${exe_name}_src} ${${exe_name}_hds})
target_include_directories(${exe_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${exe_name} PRIVATE mixal_lib)
target_link_libraries(${exe_name} PRIVATE gtest gmock gmock_main)

install(TARGETS ${exe_name}
	DESTINATION bin/Debug/tests
	CONFIGURATIONS Debug)

install(TARGETS ${exe_name}
	DESTINATION bin/Release/tests
	CONFIGURATIONS Release)
