
add_library(gl3w_Integrated INTERFACE)

find_package(gl3w CONFIG QUIET)
if (gl3w_FOUND)
	message("Found gl3w with find_package()")
	target_link_libraries(gl3w_Integrated INTERFACE unofficial::gl3w::gl3w)
endif ()

if (NOT gl3w_FOUND)
	message("Downloading & building gl3w from sources")
	add_subdirectory(gl3w)
	target_link_libraries(gl3w_Integrated INTERFACE gl3w)
endif ()

if (clang_on_msvc)
	target_compile_options(gl3w_Integrated INTERFACE
		-Wno-nonportable-system-include-path)
endif ()
