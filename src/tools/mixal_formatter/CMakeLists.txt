cmake_minimum_required(VERSION 3.2)

set(exe_name mixal_formatter)

file(GLOB_RECURSE ${exe_name}_src "source" "*.cpp")

add_executable(${exe_name} ${${exe_name}_src})

target_link_libraries(${exe_name} PRIVATE mixal_formatter_lib)

install(TARGETS ${exe_name}
	DESTINATION bin/Debug/tools
	CONFIGURATIONS Debug)

install(TARGETS ${exe_name}
	DESTINATION bin/Release/tools
	CONFIGURATIONS Release)
