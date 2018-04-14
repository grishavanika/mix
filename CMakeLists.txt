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
set_all_warnings()

# Remove unneeded configurations
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

# Set BUILD_SHARED_LIBS to build all libraries as DLLs
set(bin_folders "tools;tests")

if (clang_on_msvc)
	make_clang_work_on_windows()
endif()

if (clang_on_msvc)
	# Own code shut-up
	# TODO: move to proper project's CMake files and use 
	# target_compile_options()
	add_compile_options(
		# definition of implicit copy assignment operator for 'IIODevice'
		# is deprecated because it has a user-declared destructor [...]
		# because of usage of move-constructor for DeviceProxy class
		-Wno-deprecated
		# enumeration values not explicitly handled in switch
		# Disabling since there are proper default statements and warning
		# does not help
		-Wno-switch-enum)
endif()

if (GNU OR clang OR clang_on_msvc)
	add_compile_options(
		# MSVC <regex>
		-Wno-sign-compare)
endif()

if (MSVC AND BUILD_SHARED_LIBS)
	# class needs to have dll-interface to be used by clients of class
	add_compile_options(/wd4251)
endif()

# third party libraries
add_subdirectory(third_party)

# libraries
add_subdirectory(src/core_lib)
add_subdirectory(src/mix_lib)
add_subdirectory(src/mixal_parse_lib)
add_subdirectory(src/mixal_lib)
add_subdirectory(src/tools/mixal_format_lib)
set_target_properties(core_lib PROPERTIES FOLDER libs)
set_target_properties(mix_lib PROPERTIES FOLDER libs)
set_target_properties(mixal_lib PROPERTIES FOLDER libs)
set_target_properties(mixal_parse_lib PROPERTIES FOLDER libs)
set_target_properties(mixal_format_lib PROPERTIES FOLDER "libs/tools")

# exe
add_subdirectory(src/mixal)
add_subdirectory(src/tools/mixal_format)
set_target_properties(mixal PROPERTIES FOLDER "app")
set_target_properties(mixal_format PROPERTIES FOLDER "app/tools")

# tests
add_subdirectory(src/tests)

