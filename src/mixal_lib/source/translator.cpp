#include <mixal/translator.h>
#include <mixal/exceptions.h>

#include <mix/char_table.h>

using namespace mixal;

namespace {

template<typename Handler>
struct NamedOperation
{
	const std::string_view op;
	const Handler handler;
};

using UnaryOperationHandler = Word (*)(Word value);
using BinaryOperationHandler = Word (*)(Word lhs, Word rhs);

Word UnaryOperation_Minus(Word value)
{
	value.set_sign(mix::Sign::Negative);
	return value;
}

Word UnaryOperation_Plus(Word value)
{
	value.set_sign(mix::Sign::Positive);
	return value;
}

Word BinaryOperation_Plus(Word /*lhs*/, Word /*rhs*/)
{
	return Word{};
}

Word BinaryOperation_Minus(Word /*lhs*/, Word /*rhs*/)
{
	return Word{};
}

Word BinaryOperation_Multiply(Word /*lhs*/, Word /*rhs*/)
{
	return Word{};
}

Word BinaryOperation_Divide(Word /*lhs*/, Word /*rhs*/)
{
	return Word{};
}

Word BinaryOperation_Mod(Word /*lhs*/, Word /*rhs*/)
{
	return Word{};
}

Word BinaryOperation_Field(Word /*lhs*/, Word /*rhs*/)
{
	return Word{};
}

const NamedOperation<UnaryOperationHandler> k_unary_operations[] =
{
	{"-", &UnaryOperation_Minus},
	{"+", &UnaryOperation_Plus},
};

const NamedOperation<BinaryOperationHandler> k_binary_operations[] =
{
	{"-", &BinaryOperation_Minus},
	{"+", &BinaryOperation_Plus},
	{"*", &BinaryOperation_Multiply},
	{"/", &BinaryOperation_Divide},
	{"//", &BinaryOperation_Mod},
	{":", &BinaryOperation_Field},
};

Word CalculateOptionalUnaryOperation(const std::optional<UnaryOperation>& op, Word value)
{
	if (!op)
	{
		return value;
	}

	for (auto op_handler : k_unary_operations)
	{
		if (op_handler.op == *op)
		{
			return op_handler.handler(std::move(value));
		}
	}

	throw UnknownUnaryOperation{*op};
}

Word CalculateBinaryOperation(const BinaryOperation& op, Word lhs, Word rhs)
{
	for (auto op_handler : k_binary_operations)
	{
		if (op_handler.op == op)
		{
			return op_handler.handler(std::move(lhs), std::move(rhs));
		}
	}

	throw UnknownBinaryOperation{op};
}

} // namespace

Translator::Translator(const DefinedSymbols& symbols /*= {}*/, int current_address /*= 0*/)
	: current_address_{current_address}
	, defined_symbols_{symbols}
{
}

Word Translator::evaluate(const Text& text) const
{
	const auto& data = text.data();
	if (data.size() != Word::k_bytes_count)
	{
		throw InvalidALFText{text};
	}

	Word::BytesArray bytes;
	std::transform(data.cbegin(), data.cend(), bytes.begin(),
		[&](char ch)
	{
		bool converted = false;
		const auto char_byte = mix::CharToByte(ch, &converted);
		if (!converted)
		{
			throw InvalidALFText{text};
		}
		return char_byte;
	});

	return Word{std::move(bytes)};
}

Word Translator::evaluate(const BasicExpression& expr) const
{
	if (expr.is_current_address())
	{
		return Word{current_address_};
	}
	else if (expr.is_number())
	{
		return evaluate(expr.as_number());
	}
	else if (expr.is_symbol())
	{
		return evaluate(expr.as_symbol());
	}

	// #TODO: add UNREACHABLE() or similar macro
	assert(false);
	return Word{};
}

Word Translator::evaluate(const WValue& /*wvalue*/) const
{
	return Word{};
}

Word Translator::evaluate(const Expression& expr) const
{
	assert(!expr.tokens.empty());

	auto left_token = expr.tokens[0];
	auto value = CalculateOptionalUnaryOperation(
		left_token.unary_op, evaluate(left_token.basic_expr));

	for (std::size_t i = 1, count = expr.tokens.size(); i < count; ++i)
	{
		auto right_token = expr.tokens[i];

		assert(left_token.binary_op);
		assert(!right_token.unary_op);

		value = CalculateBinaryOperation(
			*left_token.binary_op, std::move(value), evaluate(right_token.basic_expr));

		left_token = right_token;
	}

	assert(!left_token.binary_op);

	return value;
}

Word Translator::evaluate(const Number& n) const
{
	const auto v = n.value();
	if (v > Word::k_max_abs_value)
	{
		throw TooBigWordValueError{n};
	}

	return Word{static_cast<int>(v)};
}

Word Translator::evaluate(const Symbol& symbol) const
{
	assert(!symbol.is_local());

	return defined_symbol(symbol);
}

FutureWord Translator::translate_MIX(
	Operation /*command*/,
	const APart& /*A*/, const IPart& /*I*/, const FPart& /*F*/,
	const Label& /*label*/ /*= {}*/)
{
	return {};
}

void Translator::translate_EQU(const WValue& /*value*/, const Label& /*label*/ /*= {}*/)
{
}

void Translator::translate_ORIG(const WValue& /*value*/, const Label& /*label*/ /*= {}*/)
{

}

void Translator::translate_CON(const WValue& /*value*/, const Label& /*label*/ /*= {}*/)
{

}

void Translator::translate_ALF(const Text& /*text*/, const Label& /*label*/ /*= {}*/)
{

}

void Translator::translate_END(const WValue& /*value*/, const Label& /*label*/ /*= {}*/)
{

}

void Translator::set_current_address(int address)
{
	assert(address >= 0);
	current_address_ = address;
}

int Translator::current_address() const
{
	return current_address_;
}

void Translator::define_symbol(const Symbol& symbol, const Word& value)
{
	// #TODO: only `Kind::Here` Symbols can be defined
	assert(!symbol.is_local());

	const auto it = defined_symbols_.insert(std::make_pair(symbol, value));
	const bool inserted = it.second;
	if (!inserted)
	{
		throw SymbolAlreadyDefinedError{symbol, value};
	}
}

const Word& Translator::defined_symbol(const Symbol& symbol) const
{
	// #TODO: `Kind::Here` CAN NOT be queried
	assert(!symbol.is_local());

	auto it = defined_symbols_.find(symbol);
	if (it == defined_symbols_.end())
	{
		throw SymbolIsNotDefinedError{symbol};
	}

	return it->second;
}


