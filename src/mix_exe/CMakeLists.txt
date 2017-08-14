cmake_minimum_required(VERSION 3.4)
# include(CMakePrintHelpers)

set(exe_name mix_exe)

file(GLOB_RECURSE ${exe_name}_src "source" "*.cpp")

add_definitions(-DUNICODE -D_UNICODE)
add_executable(${exe_name} ${${exe_name}_src})

target_link_libraries(${exe_name} PRIVATE mix_lib)

install(TARGETS ${exe_name}
	DESTINATION bin/Debug
	CONFIGURATIONS Debug)

install(TARGETS ${exe_name}
	DESTINATION bin/Release
	CONFIGURATIONS Release)
