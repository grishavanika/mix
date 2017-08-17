cmake_minimum_required(VERSION 3.2)
include(GenerateExportHeader)

set(lib_name mixal_format_lib)

file(GLOB_RECURSE ${lib_name}_src "source" "*.cpp")
file(GLOB_RECURSE ${lib_name}_hds "include" "*.h")

add_library(${lib_name} ${lib_type} ${${lib_name}_src} ${${lib_name}_hds})
target_link_libraries(${lib_name} PUBLIC mixal_parse_lib)

target_include_directories(${lib_name} PUBLIC include)

generate_export_header(${lib_name})
target_include_directories(${lib_name} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

if (${build_shared_lib})

	foreach (config ${CMAKE_CONFIGURATION_TYPES})
		foreach (bin_folder ${bin_folders})
			install(TARGETS ${lib_name}
				RUNTIME DESTINATION bin/${config}/${bin_folder}
				CONFIGURATIONS ${config})
		endforeach (bin_folder)
	endforeach (config)

endif ()
