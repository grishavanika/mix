include(CMakePrintHelpers)

macro(set_cpp17_standard)
	# Need to set it explicitly (even it does not work for
	# our version of CMake) to avoid overwrite
	# of C++ standard by `pybind11` third party library
	set(CMAKE_CXX_STANDARD 14)

	# Note: can be removed once CMake `CMAKE_CXX_STANDARD`
	# will support C++17 for all platforms
	if (MSVC)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest")
	else()
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

macro(set_all_warnings)
	if (only_msvc)
		add_compile_options(/W4 /WX)
	endif()

	if (NOT MSVC)
		add_compile_options(-Wall -Wextra -Wpedantic -Werror)
	endif()

	if (clang_on_msvc)
		add_compile_options(-Wall -Wextra -Werror)
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


