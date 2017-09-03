if(GNU OR clang OR clang_on_msvc)
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

new_test(core test_core)
new_test(mix test_mix)
new_test(mixal_parse test_mixal_parse)
new_test(mixal test_mixal)
