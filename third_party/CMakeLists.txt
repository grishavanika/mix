
# GoogleTest integration

# GoogleTest uses static C++ runtime by default,
# use C++ as dll instead
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

add_subdirectory(gtest)

# Using macro since most of directives should be repeated 3 times
# (for gtest, gmock_main and gmock). Using target_compile*() command
# with PUBLIC argument does not help (#TODO: investigate why)
macro(setup_gtest_lib lib_name)
	if (MSVC)
		target_compile_definitions(${lib_name} PUBLIC
			-D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING)
	endif()

	# Disable overloads for std::tr1::tuple type
	target_compile_definitions(${lib_name} PUBLIC
		-DGTEST_HAS_TR1_TUPLE=0)

	if (clang_on_msvc)
		target_compile_options(${lib_name} PUBLIC
			-Wno-undef
			-Wno-exit-time-destructors
			-Wno-format-nonliteral
			-Wno-missing-prototypes
			-Wno-missing-noreturn
			-Wno-shift-sign-overflow
			-Wno-used-but-marked-unused
			-Wno-nonportable-system-include-path
			-Wno-missing-variable-declarations
			-Wno-covered-switch-default
			-Wno-unused-member-function
			-Wno-switch-enum
			-Wno-deprecated)
	endif()

	set_target_properties(${lib_name} PROPERTIES FOLDER third_party)

endmacro()

setup_gtest_lib(gtest)
setup_gtest_lib(gmock_main)
setup_gtest_lib(gmock)

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
