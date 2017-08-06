cmake_minimum_required(VERSION 3.4)
project(mix)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Note: to avoid Visual Studio 2017 "Error: Specified cast is not valid." 
# you need to disable "Enable Faster Project Load" option
# (under Tools / Options / Text Editor / C/C++ / Experimental / Project)
# 

# TODO: wrap this into a function(s)

# TODO: correctly detect this flags on *nix
# TODO: enable c++14 or higher on *nix

set(clang_on_msvc OFF)
set(only_msvc ON)
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	set(clang_on_msvc ON)
	set(only_msvc OFF)
endif()

set(gcc OFF)
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
	set(only_msvc OFF)
	set(gcc ON)
endif()

if (gcc)
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

if (clang_on_msvc)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHa")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-argument")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-command-line-argument")
endif()

# Remove unneeded configurations
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

# third party libraries
# GoogleTest uses static C++ runtime by default
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
add_subdirectory(third_party/googletest/googlemock)
set_target_properties(gtest PROPERTIES FOLDER third_party)
set_target_properties(gmock PROPERTIES FOLDER third_party)
set_target_properties(gtest_main PROPERTIES FOLDER third_party)
set_target_properties(gmock_main PROPERTIES FOLDER third_party)

# libraries
add_subdirectory(core_lib)
add_subdirectory(mix_lib)
add_subdirectory(mixal_parser_lib)
set_target_properties(core_lib PROPERTIES FOLDER libs)
set_target_properties(mix_lib PROPERTIES FOLDER libs)
set_target_properties(mixal_parser_lib PROPERTIES FOLDER libs)

# exe
add_subdirectory(mix_exe)
set_target_properties(mix_exe PROPERTIES FOLDER app)

# tests
add_subdirectory(tests/test_core)
add_subdirectory(tests/test_mix)
add_subdirectory(tests/test_mixal_parser)
set_target_properties(test_core PROPERTIES FOLDER tests)
set_target_properties(test_mix PROPERTIES FOLDER tests)
set_target_properties(test_mixal_parser PROPERTIES FOLDER tests)

enable_testing()
add_test(NAME core COMMAND test_core)
add_test(NAME mix COMMAND test_mix)
add_test(NAME mixal_parser COMMAND test_mixal_parser)

# Note: to use it with clang you need to install latest one
# from http://llvm.org/builds/ (5.0.0 for the moment)
# CMake command:
# cmake -G "Visual Studio 14 2015 Win64" -T "LLVM-vs2014" ..

# To use it with MinGW please install STL's port from
# https://nuwen.net/mingw.html
# 
# set path=%path%;C:\Programs\mingw\bin
# cmake -G "MinGW Makefiles" ..
