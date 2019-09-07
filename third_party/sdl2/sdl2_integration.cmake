
find_package(SDL2 CONFIG REQUIRED)
add_library(SDL2_Integrated INTERFACE)
target_link_libraries(SDL2_Integrated INTERFACE SDL2::SDL2)
target_compile_definitions(SDL2_Integrated INTERFACE SDL_MAIN_HANDLED)

if (clang_on_msvc)
	target_compile_options(SDL2_Integrated INTERFACE
		-Wno-pragma-pack
		-Wno-undef
		-Wno-documentation)
endif ()

