set(lib_name core_lib)

target_collect_sources(${lib_name})

add_library(${lib_name} ${${lib_name}_files})

set_all_warnings(${lib_name} PUBLIC)

target_include_directories(${lib_name} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

generate_export_header(${lib_name})

target_include_directories(${lib_name} PUBLIC include)

target_install_lib_binaries(${lib_name})
