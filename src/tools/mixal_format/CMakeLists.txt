set(exe_name mixal_format)

target_collect_sources(${exe_name})
add_executable(${exe_name} ${${exe_name}_files})

set_all_warnings(${exe_name} PRIVATE)

target_link_libraries(${exe_name} PRIVATE mixal_parse_lib cxxopts)
target_include_directories(${exe_name} PUBLIC include)

target_install_binaries(${exe_name})


