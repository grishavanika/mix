set(lib_name mixal_format_lib)

target_collect_sources(${lib_name})

add_library(${lib_name} ${${lib_name}_files})
target_link_libraries(${lib_name} PUBLIC mixal_parse_lib)

target_include_directories(${lib_name} PUBLIC include)

generate_export_header(${lib_name})
target_include_directories(${lib_name} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

target_install_lib_binaries(${lib_name})
