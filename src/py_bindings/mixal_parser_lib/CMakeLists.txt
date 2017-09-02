set(lib_name py_mixal_parse_lib)

target_collect_sources(${lib_name})

pybind11_add_module(py_mixal_parse ${${lib_name}_files})

