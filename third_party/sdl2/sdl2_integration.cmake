
add_library(SDL2_Integrated INTERFACE)

find_package(SDL2 CONFIG QUIET)
if (TARGET SDL2::SDL2)
	message("Found SDL2 with find_package()")
	target_link_libraries(SDL2_Integrated INTERFACE SDL2::SDL2)
else ()
	message("Downloading & building SDL2 from sources")
	add_subdirectory(sdl2)
	target_link_libraries(SDL2_Integrated INTERFACE SDL2)
endif ()

target_compile_definitions(SDL2_Integrated INTERFACE SDL_MAIN_HANDLED)
if (clang_on_msvc)
	target_compile_options(SDL2_Integrated INTERFACE
		-Wno-pragma-pack
		-Wno-undef
		-Wno-documentation)
endif ()
