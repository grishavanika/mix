set(lib_name mixal_lib)

target_collect_sources(${lib_name})

add_library(${lib_name} ${${lib_name}_files})

set_all_warnings(${lib_name} PUBLIC)

if (clang_on_msvc)
	target_compile_options(${lib_name} PUBLIC
		# definition of implicit copy assignment operator
		# is deprecated because it has a user-declared destructor [...]
		# because of usage of move-constructor in derived class
		-Wno-deprecated
		# enumeration values not explicitly handled in switch
		# Disabling since there are proper default statements and warning
		# does not help
		-Wno-switch-enum)
endif()

target_link_libraries(${lib_name} PUBLIC mix_lib mixal_parse_lib)
target_include_directories(${lib_name} PUBLIC include)

generate_export_header(${lib_name})
target_include_directories(${lib_name} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

target_install_lib_binaries(${lib_name})
