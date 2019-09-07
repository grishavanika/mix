# cxxopts integration
set(CXXOPTS_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(cxxopts)

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
