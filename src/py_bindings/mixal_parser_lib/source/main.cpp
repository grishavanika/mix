#include <pybind11/pybind11.h>

namespace py = pybind11;

PYBIND11_MODULE(py_mixal_parse, m)
{
	m.def("test", []()
	{
		return 42;
	});
}
