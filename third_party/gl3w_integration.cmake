
add_library(gl3w_Integrated INTERFACE)

find_package(gl3w CONFIG QUIET)
if (TARGET unofficial::gl3w::gl3w)
	message("Found gl3w with find_package()")
	target_link_libraries(gl3w_Integrated INTERFACE unofficial::gl3w::gl3w)
endif ()

if (NOT TARGET unofficial::gl3w::gl3w)
	message("Downloading & building gl3w from sources")

	# Make sure dependencies found first
	find_package(OpenGL REQUIRED)
	# ... for code generation
	find_package(Python3 REQUIRED COMPONENTS Interpreter)

	include(FetchContent)
	include(FindPython3)
	
	FetchContent_Declare(
		gl3w_def
		SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/deps/gl3w-src"
		FULLY_DISCONNECTED ON)
	FetchContent_GetProperties(gl3w_def)
	if (NOT gl3w_def_POPULATED)
		FetchContent_Populate(gl3w_def)
	endif ()

	# Generate sources

	execute_process(COMMAND ${Python3_EXECUTABLE} "${gl3w_def_SOURCE_DIR}/gl3w_gen.py"
		RESULT_VARIABLE result
		WORKING_DIRECTORY ${gl3w_def_SOURCE_DIR})
	if (result)
		message(FATAL_ERROR "Generate step for gl3w failed: ${result}")
	endif ()

	add_library(gl3w_all
		${gl3w_def_SOURCE_DIR}/include/GL/gl3w.h
		${gl3w_def_SOURCE_DIR}/include/GL/glcorearb.h
		${gl3w_def_SOURCE_DIR}/include/KHR/khrplatform.h
		${gl3w_def_SOURCE_DIR}/src/gl3w.c)

	target_include_directories(gl3w_all PUBLIC ${gl3w_def_SOURCE_DIR}/include)
	target_include_directories(gl3w_all PUBLIC ${OPENGL_INCLUDE_DIR})
	target_link_libraries(gl3w_all PUBLIC ${OPENGL_LIBRARIES} ${CMAKE_DL_LIBS})

	set_target_properties(gl3w_all PROPERTIES FOLDER third_party)

	target_link_libraries(gl3w_Integrated INTERFACE gl3w_all)
endif ()

if (clang_on_msvc)
	target_compile_options(gl3w_Integrated INTERFACE
		-Wno-nonportable-system-include-path)
endif ()
