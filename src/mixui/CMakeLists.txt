set(exe_name mixui)

target_collect_sources(${exe_name})

add_executable(${exe_name} WIN32 ${${exe_name}_files})
target_compile_definitions(${exe_name} PRIVATE UNICODE _UNICODE)
if (${gcc})
	# Make GCC aware about wmain()
	set_target_properties(${exe_name} PROPERTIES LINK_FLAGS -municode)
endif()

set_all_warnings(${exe_name} PRIVATE)

target_link_libraries(${exe_name} PRIVATE
	mixal_lib
	mix_lib
	mixal_parse_lib
	ImGui_Integrated)

target_include_directories(${exe_name} PUBLIC include)

target_install_binaries(${exe_name})
