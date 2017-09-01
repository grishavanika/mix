cmake_minimum_required(VERSION 3.9.1)

set(lib_name core_lib)

file(GLOB_RECURSE ${lib_name}_src "source" "*.cpp")
file(GLOB_RECURSE ${lib_name}_hds "include" "*.h")

add_library(${lib_name} ${lib_type} ${${lib_name}_src} ${${lib_name}_hds})

target_include_directories(${lib_name} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

generate_export_header(${lib_name})

# From: http://stackoverflow.com/questions/11216408/cmake-dependencies-headers-between-apps-libraries-in-same-project
target_include_directories(${lib_name} PUBLIC include)

if (${BUILD_SHARED_LIBS})

	foreach (config ${CMAKE_CONFIGURATION_TYPES})
		foreach (bin_folder ${bin_folders})
			install(TARGETS ${lib_name}
				RUNTIME DESTINATION bin/${config}/${bin_folder}
				CONFIGURATIONS ${config})
		endforeach (bin_folder)
	endforeach (config)

endif ()
