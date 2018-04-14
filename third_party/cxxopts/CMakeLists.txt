
# Download and unpack cxxopt at configure time
configure_file(CMakeLists-cxxopt.txt.in cxxopt-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
	RESULT_VARIABLE result
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/cxxopt-download)
if(result)
	message(FATAL_ERROR "CMake step for cxxopt failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
	RESULT_VARIABLE result
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/cxxopt-download)
if(result)
	message(FATAL_ERROR "Build step for cxxopt failed: ${result}")
endif()

# Add cxxopt directly to our build
add_subdirectory(${CMAKE_BINARY_DIR}/cxxopt-src
				${CMAKE_BINARY_DIR}/cxxopt-build
				EXCLUDE_FROM_ALL)

