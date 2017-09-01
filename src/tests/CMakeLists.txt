cmake_minimum_required(VERSION 3.9.1)

if(clang_on_msvc)
	add_compile_options(-Wno-inconsistent-missing-override)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(test_core)
add_subdirectory(test_mix)
add_subdirectory(test_mixal)
add_subdirectory(test_mixal_parse)
set_target_properties(test_core PROPERTIES FOLDER tests)
set_target_properties(test_mix PROPERTIES FOLDER tests)
set_target_properties(test_mixal PROPERTIES FOLDER tests)
set_target_properties(test_mixal_parse PROPERTIES FOLDER tests)
