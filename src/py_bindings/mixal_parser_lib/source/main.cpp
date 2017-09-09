#include <mixal_parse/types/expression.h>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;
using namespace mixal_parse;

template<typename Data, typename T>
struct DataHolder
{
	struct Holder
	{
		Data persistent_data;
		T object;

		template<typename ObjectCreator, typename... Args>
		Holder(Data data, ObjectCreator creator, Args&&... args)
			: persistent_data{std::move(data)}
			, object{creator(persistent_data, std::forward<Args>(args)...)}
		{
		}
	};

	// ObjectCreator should be callable with signature: T (Data&, Args...)
	template<typename ObjectCreator, typename... Args>
	DataHolder(Data data, ObjectCreator creator, Args&&... args)
		: holder_{std::make_unique<Holder>(
			std::move(data), std::move(creator), std::forward<Args>(args)...)}
		, object_{&holder_->object}
	{
	}

	// #INV: this should not work since (most likely) we are getting pointer
	// to temporary object
	DataHolder(T* ref_to_existing)
		: holder_{}
		, object_{ref_to_existing}
	{
	}

	T* get() const
	{
		return object_;
	}

private:
	std::unique_ptr<Holder> holder_;
	T* object_;
};

// Strong typedef is required by `PYBIND11_DECLARE_HOLDER_TYPE()` since
// there is partial template specialization internally.
// Also, `PYBIND11_DECLARE_HOLDER_TYPE()` requieres `Holder` to be template class
// with one template parameter
// (in our use-case it's not needed, hence do defaulting).
template<typename T = Number>
struct NumberHolder : DataHolder<std::string, T>
{
	using Base = DataHolder<std::string, T>;
	using Base::Base;
};

template<typename T = Symbol>
struct SymbolHolder : DataHolder<std::string, T>
{
	using Base = DataHolder<std::string, T>;
	using Base::Base;
};

template<typename T = BasicExpression>
struct BasicExpressionHolder : DataHolder<std::string, T>
{
	using Base = DataHolder<std::string, T>;
	using Base::Base;
};

using ExpressionToken = Expression::Token;

struct ExpressionTokenData
{
	std::string unary_op;
	std::string binary_op;
};

template<typename T = ExpressionToken>
struct ExpressionTokenHolder : DataHolder<ExpressionTokenData, T>
{
	using Base = DataHolder<ExpressionTokenData, T>;
	using Base::Base;
};


PYBIND11_DECLARE_HOLDER_TYPE(T, NumberHolder<T>);
PYBIND11_DECLARE_HOLDER_TYPE(T, SymbolHolder<T>);
PYBIND11_DECLARE_HOLDER_TYPE(T, BasicExpressionHolder<T>);
PYBIND11_DECLARE_HOLDER_TYPE(T, ExpressionTokenHolder<T>);


namespace {

void DefineNumber(py::module& m)
{
	py::class_<Number, NumberHolder<>> number{m, "Number"};
	number.def(py::init([](std::string str)
	{
		return NumberHolder<>{std::move(str),
			[](std::string& py_str)
		{
			return Number{py_str.c_str()};
		}};
	}));
	
	number.def("is_zero", &Number::is_zero);
	
	number.def_property_readonly("data",	&Number::data)
		.def_property_readonly("value",		&Number::value);
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

	py::class_<Symbol, SymbolHolder<>> symbol{m, "Symbol"};

	symbol.def(py::init([](std::string str, LocalSymbolKind kind, LocalSymbolId id)
	{
		return SymbolHolder<>{std::move(str),
			[](std::string& py_str, LocalSymbolKind kind, LocalSymbolId id)
		{
			return Symbol{py_str.c_str(), kind, id};
		}, kind, id};
	}),
		"name"_a	= "",
		"kind"_a	= LocalSymbolKind::Usual,
		"id"_a		= k_invalid_local_symbol_id);

	symbol.def("is_valid",	&Symbol::is_valid)
		.def("is_local",	&Symbol::is_local);

	symbol.def_property_readonly("name",	&Symbol::name)
		.def_property_readonly("kind",		&Symbol::kind)
		.def_property_readonly("local_id",	&Symbol::local_id);
	
	symbol.def_static("from_string", [](std::string str)
	{
		return SymbolHolder<>{std::move(str),
			[](std::string& py_str)
		{
			return Symbol::FromString(py_str);
		}};
	});
}

void DefineBasicExpression(py::module& m)
{
	py::class_<BasicExpression, BasicExpressionHolder<>> basic_expression{m, "BasicExpression"};

	py::enum_<BasicExpression::Kind>(basic_expression, "Kind")
		.value("Unknown",			BasicExpression::Kind::Unknown)
		.value("Number",			BasicExpression::Kind::Number)
		.value("Symbol",			BasicExpression::Kind::Symbol)
		.value("CurrentAddress",	BasicExpression::Kind::CurrentAddress)
		.export_values();

	basic_expression
		.def(py::init([](std::string str)
	{
		return BasicExpressionHolder<>(std::move(str),
			[](std::string& py_str)
		{
			return BasicExpression{py_str.c_str()};
		});
	}))
		.def(py::init([](std::string str, BasicExpression::Kind kind)
	{
		return BasicExpressionHolder<>(std::move(str),
			[](std::string& py_str, BasicExpression::Kind kind)
		{
			return BasicExpression{py_str.c_str(), kind};
		}, kind);
	}));

	basic_expression
		.def_property_readonly("kind", &BasicExpression::kind)
		.def_property_readonly("data", &BasicExpression::data);

	basic_expression
		.def("is_valid",			&BasicExpression::is_valid)
		.def("is_number",			&BasicExpression::is_number)
		.def("is_symbol",			&BasicExpression::is_symbol)
		.def("is_current_address",	&BasicExpression::is_current_address);

	basic_expression
		.def("as_current_address",	&BasicExpression::as_current_address,
			py::return_value_policy::reference_internal)
		.def("as_number",			&BasicExpression::as_number,
			py::return_value_policy::reference_internal)
		.def("as_symbol",			&BasicExpression::as_symbol,
			py::return_value_policy::reference_internal);
}

void DefineExpressionToken(py::module& m)
{
	py::class_<ExpressionToken, ExpressionTokenHolder<>> token{m, "ExpressionToken"};

	token
		.def(py::init([](BasicExpression basic_expr)
	{
		return ExpressionToken{{}, basic_expr, {}};
	}), py::keep_alive<1, 2>{})
		.def(py::init([](std::string unary_op, BasicExpression basic_expr)
	{
		ExpressionTokenData data{std::move(unary_op)};
		return ExpressionTokenHolder<>{std::move(data),
			[](ExpressionTokenData& data, BasicExpression&& basic_expr)
		{
			return ExpressionToken{data.unary_op.c_str(), std::move(basic_expr), {}};
		}, std::move(basic_expr)};
	}), py::keep_alive<1, 3>{})
		.def(py::init([](BasicExpression basic_expr, std::string binary_op)
	{
		ExpressionTokenData data{{}, std::move(binary_op)};
		return ExpressionTokenHolder<>{std::move(data),
			[](ExpressionTokenData& data, BasicExpression&& basic_expr)
		{
			return ExpressionToken{{}, std::move(basic_expr), data.binary_op.c_str()};
		}, std::move(basic_expr)};
	}), py::keep_alive<1, 2>{})
		.def(py::init([](std::string unary_op, BasicExpression basic_expr, std::string binary_op)
	{
		ExpressionTokenData data{std::move(unary_op), std::move(binary_op)};
		return ExpressionTokenHolder<>{std::move(data),
			[](ExpressionTokenData& data, BasicExpression&& basic_expr)
		{
			return ExpressionToken{data.unary_op.c_str(), std::move(basic_expr), data.binary_op.c_str()};
		}, std::move(basic_expr)};
	}), py::keep_alive<1, 3>{});

	token.def_readonly("unary_op",	&ExpressionToken::unary_op)
		.def_readonly("basic_expr",	&ExpressionToken::basic_expr)
		.def_readonly("binary_op",	&ExpressionToken::binary_op);
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
	}), py::keep_alive<1, 2>{});

	expr.def_property_readonly("tokens", &Expression::tokens);

	expr.def("is_valid",	&Expression::is_valid)
		.def("add_token",	[](Expression& e, ExpressionToken t)
	{
		e.add_token(std::move(t));
	}, py::keep_alive<1, 2>{});
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

