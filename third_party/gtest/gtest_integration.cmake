# GoogleTest integration

macro(find_gtest found_gtest)

	find_package(GTest)
	if (GTEST_FOUND)
		add_library(GTest_Integrated INTERFACE)
		target_link_libraries(GTest_Integrated INTERFACE GTest::GTest GTest::Main)

		# If VCPKG is used we don't have GMock library target :(
		# That is the hack with assumption that gmock.lib is in the same folder as
		# gtest.lib (with other assumptions that GTest file name is gtest.lib)
		string(REPLACE "gtest" "gmock" gmock_library ${GTEST_LIBRARY})
		if (NOT EXISTS ${gmock_library})
			message(FATAL_ERROR "Assumption that gmock is near gtest library is wrong. "
				"GTest: '${GTEST_LIBRARY}'. Expected GMock: '${gmock_library}'")
		endif ()
		string(REPLACE "gtest" "gmock" gmock_library_debug ${GTEST_LIBRARY_DEBUG})
		if (NOT EXISTS ${gmock_library_debug})
			message(FATAL_ERROR "Assumption that debug gmock is near gtest library is wrong. "
				"GTest: '${GTEST_LIBRARY_DEBUG}'. Expected GMock: '${gmock_library_debug}'")
		endif ()
		target_link_libraries(GTest_Integrated INTERFACE optimized ${gmock_library})
		target_link_libraries(GTest_Integrated INTERFACE debug ${gmock_library_debug})

		set(${found_gtest} ON)
		if (MSVC)
			target_compile_options(GTest_Integrated INTERFACE
				# class needs to have dll-interface
				/wd4251
				# non-DLL-interface used as base for DLL-interface
				/wd4275)
		endif ()
	endif ()


endmacro()

# Helper for setup_gtest_from_git()
# 
# Using macro since most of directives should be repeated 3 times
# (for gtest, gmock_main and gmock). Using target_compile*() command
# with PUBLIC argument does not help (#TODO: investigate why)
macro(setup_gtest_lib lib_name)

	if (MSVC)
		target_compile_definitions(${lib_name} PUBLIC
			-D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING)
	endif ()

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
			-Wno-unused-parameter
			-Wno-deprecated)
	endif ()

	set_target_properties(${lib_name} PROPERTIES FOLDER third_party)

endmacro()

macro(setup_gtest_from_git)

	# GoogleTest uses static C++ runtime by default,
	# use C++ as dll instead
	set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
	add_subdirectory(gtest)

	setup_gtest_lib(gtest)
	setup_gtest_lib(gmock_main)
	setup_gtest_lib(gmock)

	add_library(GTest_Integrated INTERFACE)
	target_link_libraries(GTest_Integrated INTERFACE gtest gmock_main gmock)

endmacro()

set(found_gtest OFF)
if (only_msvc)
	message("Trying to integrate GTest/GMock with find_package()")
	find_gtest(found_gtest)
	if (NOT found_gtest)
		message("Failed to find GTest/GMock")
	endif ()
endif ()

if (NOT found_gtest)
	message("Integrating GTest/GMock from git")
	setup_gtest_from_git()
endif ()
