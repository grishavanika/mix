#pragma once
#include <mixal_parse/parsers_utils.h>
#include <mixal_parse/expression.h>

namespace mixal_parse {

inline ExpressionToken Token(
	std::optional<UnaryOperation> unary_op,
	BasicExpression basic_expr,
	std::optional<BinaryOperation> binary_op)
{
	return {unary_op, basic_expr, binary_op};
}

inline ExpressionToken Token(
	BasicExpression basic_expr)
{
	return Token({}, basic_expr, {});
}

inline ExpressionToken BinaryToken(
	BasicExpression basic_expr,
	BinaryOperation binary_op)
{
	return Token({}, basic_expr, binary_op);
}

inline ExpressionToken UnaryToken(
	UnaryOperation unary_op,
	BasicExpression basic_expr)
{
	return Token(unary_op, basic_expr, {});
}

struct ExpressionBuilder
{
	Expression build() const
	{
		return expr_;
	}

	ExpressionBuilder& add(ExpressionToken token)
	{
		expr_.tokens.push_back(std::move(token));
		return *this;
	}

	template<typename... Exprs>
	static Expression Build(const Exprs&... exprs)
	{
		ExpressionBuilder expr_builder;
		(void)std::initializer_list<int>{((void)expr_builder.add(exprs), 0)...};
		return expr_builder.build();
	}

private:
	Expression expr_;
};

inline ExpressionBuilder Expr()
{
	return ExpressionBuilder{};
}

struct WTokenBuilder
{
	template<typename... Exprs>
	WTokenBuilder& expression_is(const Exprs&... exprs)
	{
		token_.expression = ExpressionBuilder::Build(exprs...);
		return *this;
	}

	template<typename... Exprs>
	WTokenBuilder& field_is(const Exprs&... exprs)
	{
		token_.field = ExpressionBuilder::Build(exprs...);
		return *this;
	}

	WValueToken build() const
	{
		return token_;
	}

	template<typename... Builders>
	static WValue Build(const Builders&... builders)
	{
		WValue value;
		(void)std::initializer_list<int>{((void)value.tokens.push_back(builders.build()), 0)...};
		return value;
	}

private:
	WValueToken token_;
};

inline WTokenBuilder WToken()
{
	return WTokenBuilder{};
}

struct MIXALOpBuilder
{
	std::optional<WValue> w_value;
	std::optional<std::string_view> alf_text;

	MIXALOpBuilder& ALF_text_is(const std::string_view& text)
	{
		assert(!alf_text);
		alf_text = text;
		return *this;
	}

	MIXALOpBuilder& token(WTokenBuilder& token_builder)
	{
		if (!w_value)
		{
			w_value = WValue{};
		}

		w_value->tokens.push_back(token_builder.build());
		return *this;
	}
};

inline MIXALOpBuilder MIXAL()
{
	return MIXALOpBuilder{};
}

struct MIXOpBuilder
{
	std::optional<Expression> index_expr;
	std::optional<Expression> field_expr;

	std::optional<Expression> address_expr;
	std::optional<WValue> address_w_value;

	template<typename... Exprs>
	MIXOpBuilder& address_is(const ExpressionToken& expr0, const Exprs&... exprs)
	{
		address_expr = ExpressionBuilder::Build(expr0, exprs...);
		return *this;
	}

	template<typename... WBuilders>
	MIXOpBuilder& address_is(const WTokenBuilder& w_builder, const WBuilders&... builders)
	{
		address_w_value = WTokenBuilder::Build(w_builder, builders...);
		return *this;
	}

	template<typename... Exprs>
	MIXOpBuilder& index_is(const Exprs&... exprs)
	{
		index_expr = ExpressionBuilder::Build(exprs...);
		return *this;
	}

	template<typename... Exprs>
	MIXOpBuilder& field_is(const Exprs&... exprs)
	{
		field_expr = ExpressionBuilder::Build(exprs...);
		return *this;
	}

};

inline MIXOpBuilder MIX()
{
	return MIXOpBuilder{};
}

} // namespace mixal_parse
