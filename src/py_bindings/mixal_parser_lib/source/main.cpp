#include <mixal_parse/types/expression.h>

#include <pybind11/pybind11.h>

namespace py = pybind11;
using namespace mixal_parse;

namespace {

void DefineNumber(py::module& m)
{
	py::class_<Number>(m, "Number")
		.def(py::init<const std::string_view&>())
		.def("is_zero", &Number::is_zero)
		.def_property_readonly("data", &Number::data)
		.def_property_readonly("value", &Number::value);
}

void DefineSymbol(py::module& m)
{
	py::class_<Symbol>(m, "Symbol")
		.def(py::init<const std::string_view&>());
}

void DefineBasicExpression(py::module& m)
{
	py::class_<BasicExpression> basic_expression(m, "BasicExpression");
	basic_expression
		.def(py::init<>())
		.def(py::init<const std::string_view&>())
		.def(py::init<const char*>())
		.def(py::init<const std::string_view&, BasicExpression::Kind>());

	basic_expression
		.def_property_readonly("kind", &BasicExpression::kind)
		.def_property_readonly("data", &BasicExpression::data);

	basic_expression
		.def("is_valid", &BasicExpression::is_valid)
		.def("is_number", &BasicExpression::is_number)
		.def("is_symbol", &BasicExpression::is_symbol)
		.def("is_current_address", &BasicExpression::is_current_address);

	basic_expression
		.def("as_current_address", &BasicExpression::as_current_address)
		.def("as_number", &BasicExpression::as_number)
		.def("as_symbol", &BasicExpression::as_symbol);

	py::enum_<BasicExpression::Kind>(basic_expression, "Kind")
		.value("Unknown", BasicExpression::Kind::Unknown)
		.value("Number", BasicExpression::Kind::Number)
		.value("Symbol", BasicExpression::Kind::Symbol)
		.value("CurrentAddress", BasicExpression::Kind::CurrentAddress)
		.export_values();
}

void DefineExpressionToken(py::module& m)
{
	using ExpressionToken = Expression::Token;
	py::class_<ExpressionToken>(m, "ExpressionToken")
		.def_readwrite("unary_op", &ExpressionToken::unary_op)
		.def_readwrite("binary_op", &ExpressionToken::binary_op)
		.def_readwrite("basic_expr", &ExpressionToken::basic_expr);
}

void DefineExpression(py::module& m)
{
	py::class_<Expression>(m, "Expression")
		.def_property_readonly("tokens", &Expression::tokens)
		.def("is_valid", &Expression::is_valid)
		.def("add_token", [](Expression& e, Expression::Token t)
	{
		e.add_token(std::move(t));
	});
}

} // namespace

PYBIND11_MODULE(py_mixal_parse, m)
{
	DefineNumber(m);
	DefineSymbol(m);
	DefineBasicExpression(m);
	DefineExpression(m);
}

