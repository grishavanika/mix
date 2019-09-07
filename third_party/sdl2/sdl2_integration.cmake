
find_package(SDL2 CONFIG REQUIRED)

add_library(SDL2_Integrated INTERFACE)
target_compile_definitions(SDL2_Integrated INTERFACE SDL_MAIN_HANDLED)

# Vcpkg has SDL2::SDL2 
if (TARGET SDL2::SDL2)
	target_link_libraries(SDL2_Integrated INTERFACE SDL2::SDL2)
else ()
	# ... others (apt-get install libsdl2-dev) go old way
	target_include_directories(SDL2_Integrated INTERFACE ${SDL2_INCLUDE_DIRS})
	target_link_libraries(SDL2_Integrated INTERFACE ${SDL2_LIBRARIES})
endif ()

if (clang_on_msvc)
	target_compile_options(SDL2_Integrated INTERFACE
		-Wno-pragma-pack
		-Wno-undef
		-Wno-documentation)
endif ()

