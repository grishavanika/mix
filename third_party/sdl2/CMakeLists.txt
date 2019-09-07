
# Download and unpack sdl2 at configure time
configure_file(CMakeLists-sdl2.txt.in sdl2-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
	RESULT_VARIABLE result
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/sdl2-download)
if(result)
	message(FATAL_ERROR "CMake step for sdl2 failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
	RESULT_VARIABLE result
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/sdl2-download)
if(result)
	message(FATAL_ERROR "Build step for sdl2 failed: ${result}")
endif()

# Add sdl2 directly to our build
add_subdirectory(${CMAKE_BINARY_DIR}/sdl2-src
				${CMAKE_BINARY_DIR}/sdl2-build
				EXCLUDE_FROM_ALL)

set_target_properties(SDL2 PROPERTIES FOLDER third_party)
