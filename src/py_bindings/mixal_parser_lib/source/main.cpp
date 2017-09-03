#include <mixal_parse/types/expression.h>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;
using namespace mixal_parse;

namespace {

// #INV: read about return values policies:
// http://pybind11.readthedocs.io/en/master/advanced/functions.html?highlight=repr#return-value-policies
// 
struct NumberHolder : std::string, Number
{
	NumberHolder(std::string str)
		: std::string{std::move(str)}
		, Number{c_str()} {}

	NumberHolder(Number number)
		: NumberHolder{std::string{number.data()}} {}

	NumberHolder(NumberHolder&& rhs)
		: std::string{std::move(static_cast<std::string&>(rhs))}
		, Number{c_str()}
	{
	}

	NumberHolder(const NumberHolder& rhs) = delete;
	NumberHolder& operator=(const NumberHolder& rhs) = delete;
	NumberHolder& operator=(NumberHolder&&) = delete;
};

struct SymbolHolder : std::string, Symbol
{
	SymbolHolder(std::string name, LocalSymbolKind kind, LocalSymbolId id)
		: std::string{std::move(name)}
		, Symbol{c_str(), kind, id} {}

	SymbolHolder(Symbol symbol)
		: SymbolHolder{std::string{symbol.name()}, symbol.kind(), symbol.local_id()} {}

	SymbolHolder(SymbolHolder&& rhs)
		: std::string{std::move(static_cast<std::string&>(rhs))}
		, Symbol{c_str(), rhs.kind(), rhs.local_id()} {}

	SymbolHolder(const SymbolHolder& rhs) = delete;
	SymbolHolder& operator=(const SymbolHolder&) = delete;
	SymbolHolder& operator=(SymbolHolder&&) = delete;
};

struct BasicExpressionHolder : std::string, BasicExpression
{
	BasicExpressionHolder() = default;

	BasicExpressionHolder(std::string str)
		: std::string{std::move(str)}
		, BasicExpression{c_str()} {}

	BasicExpressionHolder(std::string str, Kind kind)
		: std::string{std::move(str)}
		, BasicExpression{c_str(), kind} {}

	BasicExpressionHolder(BasicExpressionHolder&& rhs)
		: std::string{std::move(static_cast<std::string&>(rhs))}
		, BasicExpression{c_str()} {}

	BasicExpressionHolder(const BasicExpressionHolder& rhs) = delete;
	BasicExpressionHolder& operator=(const BasicExpressionHolder&) = delete;
	BasicExpressionHolder& operator=(BasicExpressionHolder&&) = delete;
};

struct Holder
{
	std::string unary_op_data;
	std::string binary_op_data;

	std::string_view op_1_data() { return unary_op_data; }
	std::string_view op_2_data() { return binary_op_data; }
};

struct ExpressionToken : Holder, Expression::Token
{
	ExpressionToken(BasicExpression expr = {})
	{
		basic_expr = std::move(expr);
	}

	ExpressionToken(std::string unary, BasicExpression expr)
		: Holder{std::move(unary), std::string{}}
	{
		unary_op = op_1_data();
		basic_expr = std::move(expr);
	}

	ExpressionToken(BasicExpression expr, std::string binary)
		: Holder{std::string{}, std::move(binary)}
	{
		basic_expr = std::move(expr);
		binary_op = op_2_data();
	}

	ExpressionToken(std::string unary, BasicExpression expr, std::string binary)
		: Holder{std::move(unary), std::move(binary)}
	{
		unary_op = op_1_data();
		binary_op = op_2_data();
		basic_expr = std::move(expr);
	}

	ExpressionToken(ExpressionToken&& rhs)
		: Holder{std::move(static_cast<Holder&>(rhs))}
	{
		// `basic_expr` can be moved. Ignore this optimization now.
		// The way we manage life-time now is wrong, I'm sure
		set_from_other(rhs);
	}

	ExpressionToken(const ExpressionToken& rhs)
		: Holder{static_cast<const Holder&>(rhs)}
	{
		set_from_other(rhs);
	}

private:
	void set_from_other(const ExpressionToken& rhs)
	{
		basic_expr = rhs.basic_expr;

		if (rhs.unary_op)
		{
			unary_op = op_1_data();
		}
		if (rhs.binary_op)
		{
			binary_op = op_2_data();
		}
	}
};

void DefineNumber(py::module& m)
{
	py::class_<NumberHolder> number{m, "Number"};
	number.def(py::init<std::string>());
	
	number.def("is_zero", &NumberHolder::is_zero);
	
	number.def_property_readonly("data",	&NumberHolder::Number::data)
		.def_property_readonly("value",		&NumberHolder::value);
}

void DefineSymbol(py::module& m)
{
	using namespace py;

	py::enum_<LocalSymbolKind>(m, "LocalSymbolKind")
		.value("Unknown",	LocalSymbolKind::Unknown)
		.value("Usual",		LocalSymbolKind::Usual)
		.value("Here",		LocalSymbolKind::Here)
		.value("Forward",	LocalSymbolKind::Forward)
		.value("Backward",	LocalSymbolKind::Backward)
		.export_values();

	py::class_<SymbolHolder> symbol{m, "Symbol"};
	symbol.def(py::init<std::string, LocalSymbolKind, LocalSymbolId>(),
		"name"_a	= "",
		"kind"_a	= LocalSymbolKind::Usual,
		"id"_a		= k_invalid_local_symbol_id);

	symbol.def("is_valid",	&SymbolHolder::is_valid)
		.def("is_local",	&SymbolHolder::is_local);

	symbol.def_property_readonly("name",	&SymbolHolder::name)
		.def_property_readonly("kind",		&SymbolHolder::kind)
		.def_property_readonly("local_id",	&SymbolHolder::local_id);
	
	symbol.def_static("from_string", [](std::string_view str)
	{
		return SymbolHolder{Symbol::FromString(str)};
	});
}

void DefineBasicExpression(py::module& m)
{
	py::class_<BasicExpressionHolder> basic_expression{m, "BasicExpression"};
	basic_expression
		.def(py::init<>())
		.def(py::init<std::string>())
		.def(py::init<std::string&, BasicExpression::Kind>());

	basic_expression
		.def_property_readonly("kind", &BasicExpressionHolder::kind)
		.def_property_readonly("data", &BasicExpressionHolder::BasicExpression::data);

	basic_expression
		.def("is_valid",			&BasicExpressionHolder::is_valid)
		.def("is_number",			&BasicExpressionHolder::is_number)
		.def("is_symbol",			&BasicExpressionHolder::is_symbol)
		.def("is_current_address",	&BasicExpressionHolder::is_current_address);

	basic_expression
		.def("as_current_address",	&BasicExpressionHolder::as_current_address)
		.def("as_number",			[](BasicExpressionHolder& self)
	{
		return NumberHolder{self.as_number()};
	})
		.def("as_symbol", [](BasicExpressionHolder& self)
	{
		return SymbolHolder{self.as_symbol()};
	});

	py::enum_<BasicExpression::Kind>(basic_expression, "Kind")
		.value("Unknown",			BasicExpression::Kind::Unknown)
		.value("Number",			BasicExpression::Kind::Number)
		.value("Symbol",			BasicExpression::Kind::Symbol)
		.value("CurrentAddress",	BasicExpression::Kind::CurrentAddress)
		.export_values();
}

void DefineExpressionToken(py::module& m)
{
	using namespace py;

	py::class_<ExpressionToken> token{m, "ExpressionToken"};

	token
		.def(py::init<>())
		.def(py::init<BasicExpression>())
		.def(py::init<std::string, BasicExpression>())
		.def(py::init<BasicExpression, std::string>())
		.def(py::init<std::string, BasicExpression, std::string>());
	
	token.def_readwrite("unary_op",		&ExpressionToken::unary_op)
		.def_readwrite("basic_expr",	&ExpressionToken::basic_expr)
		.def_readwrite("binary_op",		&ExpressionToken::binary_op);
}

void DefineExpression(py::module& m)
{
	py::class_<Expression> expr{m, "Expression"};
	
	expr.def(py::init([](std::vector<ExpressionToken>&& tokens)
	{
		Expression expr;
		for (auto& token : tokens)
		{
			expr.add_token(std::move(token));
		}
		return expr;
	}), py::return_value_policy::move);

	expr.def_property_readonly("tokens", &Expression::tokens);

	expr.def("is_valid",	&Expression::is_valid)
		.def("add_token",	[](Expression& e, Expression::Token t)
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
	DefineExpressionToken(m);
	DefineExpression(m);
}

