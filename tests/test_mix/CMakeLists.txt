cmake_minimum_required(VERSION 3.4)

set(exe_name test_mix)

file(GLOB_RECURSE ${exe_name}_src "source" "*.cpp")

add_definitions(-DUNICODE -D_UNICODE)
add_executable(${exe_name} ${${exe_name}_src})

target_link_libraries(${exe_name} PRIVATE mix_lib)
target_link_libraries(${exe_name} PRIVATE gtest gmock gmock_main)

install(TARGETS ${exe_name}
	DESTINATION bin/tests/Debug
	CONFIGURATIONS Debug)

install(TARGETS ${exe_name}
	DESTINATION bin/tests/Release
	CONFIGURATIONS Release)



