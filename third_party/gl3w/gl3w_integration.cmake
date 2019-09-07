
find_package(gl3w CONFIG REQUIRED)
add_library(gl3w_Integrated INTERFACE)
target_link_libraries(gl3w_Integrated INTERFACE unofficial::gl3w::gl3w)

if (clang_on_msvc)
	target_compile_options(gl3w_Integrated INTERFACE
		-Wno-nonportable-system-include-path)
endif ()
