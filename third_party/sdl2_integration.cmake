add_library(SDL2_Integrated INTERFACE)

find_package(SDL2 CONFIG QUIET)
if (TARGET SDL2::SDL2)
	message("Found SDL2 with find_package()")
	target_link_libraries(SDL2_Integrated INTERFACE SDL2::SDL2)
else ()
	message("Downloading & building SDL2 from sources")

	include(FetchContent)

	FetchContent_Declare(
		sdl2
		SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/deps/sdl2-src"
		FULLY_DISCONNECTED ON)
	FetchContent_GetProperties(sdl2)
	if (NOT sdl2_POPULATED)
		FetchContent_Populate(sdl2)
	endif ()

	set(SDL_STATIC OFF CACHE BOOL "Build a static version of the library" FORCE)

	add_subdirectory(${sdl2_SOURCE_DIR} ${sdl2_BINARY_DIR} EXCLUDE_FROM_ALL)

	set_target_properties(SDL2 PROPERTIES FOLDER third_party)
	target_link_libraries(SDL2_Integrated INTERFACE SDL2)
endif ()

target_compile_definitions(SDL2_Integrated INTERFACE SDL_MAIN_HANDLED)
if (clang_on_msvc)
	target_compile_options(SDL2_Integrated INTERFACE
		-Wno-pragma-pack
		-Wno-undef
		-Wno-documentation
		-Wno-implicit-function-declaration)
endif ()
