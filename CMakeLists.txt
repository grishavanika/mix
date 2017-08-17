cmake_minimum_required(VERSION 3.2)
project(mix)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(build_shared_lib OFF)
if (build_shared_lib)
	set(lib_type SHARED)
else()
	set(lib_type STATIC)
endif()

set(bin_folders "tools;tests")

# Note: to avoid Visual Studio 2017 "Error: Specified cast is not valid." 
# you need to disable "Enable Faster Project Load" option
# (under Tools / Options / Text Editor / C/C++ / Experimental / Project)
# 

# TODO: wrap this into a function(s)

# TODO: correctly detect this flags on *nix
# TODO: enable c++14 or higher on *nix

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

if (MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")
endif()

if (only_msvc)
	# Force to always compile with W4 and WX
	if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
		string(REGEX REPLACE "/W[0-4]" "/WX /W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	else()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX")
	endif()
endif()

if (NOT MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")
endif()

if (clang_on_msvc)
	# almost all of these are from VC system headers
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHa")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-argument")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-command-line-argument")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-ignored-attributes")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-microsoft-template")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-dllimport-static-field-def")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-dangling-else")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-logical-op-parentheses")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-local-typedef")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-invalid-noreturn")

	# investigate why this happens with Clang toolset
	add_definitions(-D_HAS_CXX17=1)
	include_directories(SYSTEM "C:/Programs/Microsoft Visual Studio/2017/Community/VC/Tools/MSVC/14.10.25017/include")
endif()

if (MSVC)
	# class needs to have dll-interface to be used by clients of class
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4251")
endif ()

# Remove unneeded configurations
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

message("Options: ${CMAKE_CXX_FLAGS}")

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
add_subdirectory(src/tests/test_core)
add_subdirectory(src/tests/test_mix)
add_subdirectory(src/tests/test_mixal)
add_subdirectory(src/tests/test_mixal_parse)
set_target_properties(test_core PROPERTIES FOLDER tests)
set_target_properties(test_mix PROPERTIES FOLDER tests)
set_target_properties(test_mixal PROPERTIES FOLDER tests)
set_target_properties(test_mixal_parse PROPERTIES FOLDER tests)

enable_testing()
add_test(NAME core COMMAND test_core)
add_test(NAME mix COMMAND test_mix)
add_test(NAME mixal_parse COMMAND test_mixal_parse)
add_test(NAME mixal COMMAND test_mixal)

# Note: to use it with clang you need to install latest one
# from http://llvm.org/builds/ (5.0.0 for the moment)
# CMake command:
# cmake -G "Visual Studio 14 2015 Win64" -T "LLVM-vs2014" ..

# To use it with MinGW please install STL's port from
# https://nuwen.net/mingw.html
# 
# set path=%path%;C:\Programs\mingw\bin
# cmake -G "MinGW Makefiles" ..
