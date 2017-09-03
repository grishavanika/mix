cmake_minimum_required(VERSION 3.2)
project(mix)

include(CheckCXXCompilerFlag)
include(GenerateExportHeader)
include(CTest)
include(scripts/cmake/utils.cmake)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set_cpp17_standard()

# Remove unneeded configurations
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

# Set BUILD_SHARED_LIBS to build all libraries as DLLs

set(bin_folders "tools;tests")

detect_compilers()

set_all_warnings()

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

if (GNU OR clang OR clang_on_msvc)
	add_compile_options(
		-Wno-sign-compare # MSVC <regex>
		-Wno-delete-non-virtual-dtor # third-party cxxopts
		)
endif ()

if (MSVC AND BUILD_SHARED_LIBS)
	# class needs to have dll-interface to be used by clients of class
	add_compile_options(/wd4251)
endif ()

# third party libraries
# GoogleTest uses static C++ runtime by default
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
add_subdirectory(third_party/googletest/googlemock)

set(CXXOPTS_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(third_party/cxxopts)

option(build_python_libs "Build Python ports of MIX[AL]" OFF)

if (build_python_libs)
	add_subdirectory(third_party/pybind11)
	add_subdirectory(src/py_bindings)
endif()

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

add_subdirectory(src/tests)

