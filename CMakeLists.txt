cmake_minimum_required(VERSION 3.8)
project(mix)

include(CheckCXXCompilerFlag)
include(GenerateExportHeader)
include(CTest)
include(scripts/cmake/utils.cmake)
include(scripts/cmake/clang_msvc_integration.cmake)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

detect_compilers()
set_cpp17_standard()

# Remove unneeded configurations
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

if (clang_on_msvc)
	make_clang_work_on_windows()
endif()

if (GNU OR clang OR clang_on_msvc)
	add_compile_options(
		# MSVC <regex>
		-Wno-sign-compare)
endif()

if (MSVC AND BUILD_SHARED_LIBS)
	# class needs to have dll-interface to be used by clients of class
	add_compile_options(/wd4251)
	# non dll-interface class used as base for dll-interface class
	add_compile_options(/wd4275)
endif()

# third party libraries
add_subdirectory(third_party)

# libraries
add_subdirectory(src/core_lib)
add_subdirectory(src/mix_lib)
add_subdirectory(src/mixal_parse_lib)
add_subdirectory(src/mixal_lib)
set_target_properties(core_lib PROPERTIES FOLDER libs)
set_target_properties(mix_lib PROPERTIES FOLDER libs)
set_target_properties(mixal_lib PROPERTIES FOLDER libs)
set_target_properties(mixal_parse_lib PROPERTIES FOLDER libs)

# exe
add_subdirectory(src/mixal)
add_subdirectory(src/tools/mixal_format)
set_target_properties(mixal PROPERTIES FOLDER "app")
set_target_properties(mixal_format PROPERTIES FOLDER "app/tools")

# tests
add_subdirectory(src/tests)

