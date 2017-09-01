cmake_minimum_required(VERSION 3.9.1)

include(GenerateExportHeader)

set(lib_name mix_lib)

file(GLOB_RECURSE ${lib_name}_src "source" "*.cpp")
file(GLOB_RECURSE ${lib_name}_hds "include" "*.h")

# See internal/valarray_register_helpers.h std::copy() call.
# Can be removed once valarray_register_helpers.h will be removed
add_definitions(-D_SCL_SECURE_NO_WARNINGS)
add_library(${lib_name} ${lib_type} ${${lib_name}_src} ${${lib_name}_hds})

target_include_directories(${lib_name} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

generate_export_header(${lib_name})

target_link_libraries(${lib_name} PUBLIC core_lib)

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
