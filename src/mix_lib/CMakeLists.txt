set(lib_name mix_lib)

target_collect_sources(${lib_name})

add_library(${lib_name} ${${lib_name}_files})

# See internal/valarray_register_helpers.h std::copy() call.
# Can be removed once valarray_register_helpers.h will be removed
target_compile_definitions(${lib_name} PRIVATE -D_SCL_SECURE_NO_WARNINGS)

target_include_directories(${lib_name} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

generate_export_header(${lib_name})

target_link_libraries(${lib_name} PUBLIC core_lib)

target_include_directories(${lib_name} PUBLIC include)

target_install_lib_binaries(${lib_name})

