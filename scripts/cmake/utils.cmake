include(CMakePrintHelpers)

macro(set_cpp17_standard)
	set(CXX_STANDARD_REQUIRED ON)
	set(CMAKE_CXX_STANDARD 17)

	# And `CMAKE_CXX_STANDARD` does not work for Clang on Windows
	if (clang_on_msvc)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")
	endif()

endmacro()

macro(detect_compilers)
	set(clang_on_msvc OFF)
	set(clang OFF)
	set(only_msvc ${MSVC})
	set(gcc ${GNU})

	if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
		set(clang ON)
		set(clang_on_msvc ${MSVC})
		set(only_msvc OFF)
	endif()
endmacro()

macro(set_all_warnings target visibility)
	if (only_msvc)
		target_compile_options(${target} ${visibility}
			/W4 /WX)
	endif()

	if (NOT MSVC)
		target_compile_options(${target} ${visibility}
			-Wall -Wextra -Wpedantic -Werror)
	endif()

	if (clang_on_msvc)
		target_compile_options(${target} ${visibility}
			-Wall -Wextra -Werror)
	endif()
endmacro()

# set PCH for VS project
# https://stackoverflow.com/questions/148570/using-pre-compiled-headers-with-cmake

# Note: relies on ${only_msvc} global variable
function(set_target_precompiled_header target pc_header pc_source)
  if (only_msvc)
     set_target_properties(${target} PROPERTIES COMPILE_FLAGS "/Yu${pc_header}")
     set_source_files_properties(${pc_source} PROPERTIES COMPILE_FLAGS "/Yc${pc_header}")
  endif ()
endfunction()

# Note: relies on ${bin_folders} global variable
function(target_install_lib_binaries target)
	if(${BUILD_SHARED_LIBS})	
		foreach(config ${CMAKE_CONFIGURATION_TYPES})
			foreach(bin_folder ${bin_folders})
				install(TARGETS ${target}
					RUNTIME DESTINATION bin/${config}/${bin_folder}
					CONFIGURATIONS ${config})
			endforeach(bin_folder)
		endforeach(config)
	endif()
endfunction()

function(target_install_binaries target)
	foreach(config ${CMAKE_CONFIGURATION_TYPES})
		install(TARGETS ${target}
			RUNTIME DESTINATION bin/${config}
			CONFIGURATIONS ${config})
	endforeach(config)
endfunction()


macro(target_collect_sources target)

	file(GLOB_RECURSE
		${target}_files
		"${CMAKE_CURRENT_SOURCE_DIR}"
		"*.cpp" "*.h" "*.inl")

endmacro()

macro(new_test name command)
	get_filename_component(base_install_folder ${CMAKE_INSTALL_PREFIX} ABSOLUTE)

	foreach(config ${CMAKE_CONFIGURATION_TYPES})
		set(install_folder ${base_install_folder}/bin/${config}/tests)

		add_test(NAME
			${name}_${config}
			COMMAND ${command}
			CONFIGURATIONS ${config}
			WORKING_DIRECTORY ${install_folder})
	endforeach()

endmacro()


