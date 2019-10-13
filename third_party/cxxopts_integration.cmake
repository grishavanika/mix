include(FetchContent)

FetchContent_Declare(
	cxxopts
	SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/deps/cxxopts-src"
	FULLY_DISCONNECTED ON)
FetchContent_GetProperties(cxxopts)
if (NOT cxxopts_POPULATED)
	FetchContent_Populate(cxxopts)
endif ()

# cxxopts integration
set(CXXOPTS_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(${cxxopts_SOURCE_DIR} ${cxxopts_BINARY_DIR} EXCLUDE_FROM_ALL)

if (clang_on_msvc)
	target_compile_options(cxxopts INTERFACE
		-Wno-non-virtual-dtor
		-Wno-exit-time-destructors
		-Wno-covered-switch-default
		-Wno-conversion
		-Wno-deprecated)
endif()

if (GNU OR clang OR clang_on_msvc)
	target_compile_options(cxxopts INTERFACE
		-Wno-delete-non-virtual-dtor)
endif()

if (MSVC)
	target_compile_options(cxxopts INTERFACE
		# conversion from 'size_t' to 'type', possible loss of data
		/wd4267
		# '<': signed/unsigned mismatch
		/wd4018)
endif()
