# See https://github.com/skaslev/gl3w/blob/master/CMakeLists.txt
# TL;DR:
#  * Download & generate gl3w
#  * Set properly dependencies (including OpenGL)

# Download and gl3w gl3w at configure time
configure_file(CMakeLists-gl3w.txt.in gl3w-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
	RESULT_VARIABLE result
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/gl3w-download)
if(result)
	message(FATAL_ERROR "CMake (configure) step for gl3w failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
	RESULT_VARIABLE result
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/gl3w-download)
if(result)
	message(FATAL_ERROR "CMake (download) step for gl3w failed: ${result}")
endif()

# Generate sources
set(gl3w_root ${CMAKE_BINARY_DIR}/gl3w-src)
execute_process(COMMAND python "${gl3w_root}/gl3w_gen.py"
	RESULT_VARIABLE result
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/gl3w-src)
if (result)
	message(FATAL_ERROR "Generate step for gl3w failed: ${result}")
endif ()

find_package(OpenGL REQUIRED)

add_library(gl3w
	${gl3w_root}/include/GL/gl3w.h
	${gl3w_root}/include/GL/glcorearb.h
	${gl3w_root}/include/KHR/khrplatform.h
	${gl3w_root}/src/gl3w.c)

target_include_directories(gl3w PUBLIC ${gl3w_root}/include)
target_include_directories(gl3w PUBLIC ${OPENGL_INCLUDE_DIR})
target_link_libraries(gl3w PUBLIC ${OPENGL_LIBRARIES} ${CMAKE_DL_LIBS})

set_target_properties(${gl3w} PROPERTIES FOLDER third_party)
