target_collect_sources(py_mixal_parse)

pybind11_add_module(py_mixal_parse ${py_mixal_parse_files})

target_link_libraries(py_mixal_parse PRIVATE mixal_parse_lib)

target_compile_definitions(py_mixal_parse PRIVATE -D_CRT_SECURE_NO_WARNINGS)

function(pybind_copy_binaries target)
	foreach(config ${CMAKE_CONFIGURATION_TYPES})
		install(TARGETS ${target}
			DESTINATION bin/${config}/py
			CONFIGURATIONS ${config})
	endforeach(config)
endfunction()


pybind_copy_binaries(py_mixal_parse)
