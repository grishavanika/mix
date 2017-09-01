cmake_minimum_required(VERSION 3.2)
include(CheckCXXCompilerFlag)
include(GenerateExportHeader)
include(scripts/cmake/utils.cmake)

project(mix)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Note: can be removed once CMake `CMAKE_CXX_STANDARD`
# will support C++17 for all platforms
if (MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")
endif()

# Remove unneeded configurations
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

# Set BUILD_SHARED_LIBS to build all libraries as DLLs

set(bin_folders "tools;tests")

set(clang_on_msvc OFF)
set(only_msvc ${MSVC})
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND MSVC)
	set(clang_on_msvc ON)
	set(only_msvc OFF)
endif()

set(gcc OFF)
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
	set(only_msvc OFF)
	set(gcc ON)
endif()

if (only_msvc)
	add_compile_options(/W4 /WX)
endif()

if (NOT MSVC)
	add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

if (clang_on_msvc)
	add_compile_options(/EHa)

	# almost all of these are from VC system headers
	add_compile_options(
		-Wno-deprecated-declarations
		-Wno-unknown-argument
		-Wno-unused-command-line-argument
		-Wno-ignored-attributes
		-Wno-microsoft-template
		-Wno-dllimport-static-field-def
		-Wno-dangling-else
		-Wno-logical-op-parentheses
		-Wno-unused-local-typedef
		-Wno-invalid-noreturn)

	#INV: why this happens with Clang toolset ?
	add_definitions(-D_HAS_CXX17=1)
	include_directories(SYSTEM "C:/Programs/Microsoft Visual Studio/2017/Community/VC/Tools/MSVC/14.11.25503/include")
endif()

if (MSVC AND BUILD_SHARED_LIBS)
	# class needs to have dll-interface to be used by clients of class
	add_compile_options(/wd4251)
endif ()

# third party libraries
# GoogleTest uses static C++ runtime by default
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
add_subdirectory(third_party/googletest/googlemock)

set_target_properties(gtest PROPERTIES FOLDER third_party)
set_target_properties(gmock PROPERTIES FOLDER third_party)
set_target_properties(gtest_main PROPERTIES FOLDER third_party)
set_target_properties(gmock_main PROPERTIES FOLDER third_party)

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
# Can't do this in subdirectory since cctest will fail to run tests
enable_testing()

add_subdirectory(src/tests)

