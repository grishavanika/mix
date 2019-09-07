
# Download and unpack imgui at configure time
configure_file(CMakeLists-imgui.txt.in imgui-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
	RESULT_VARIABLE result
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/imgui-download)
if(result)
	message(FATAL_ERROR "CMake step for imgui failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
	RESULT_VARIABLE result
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/imgui-download)
if(result)
	message(FATAL_ERROR "Build step for imgui failed: ${result}")
endif()
