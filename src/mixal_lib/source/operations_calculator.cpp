#include <mixal/operations_calculator.h>
#include <mixal/exceptions.h>

namespace mixal {

////////////////////////////////////////////////////////////////////////////////
// #TODO: handle more cleanly overflow/underflow

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
	return value;
}

Word BinaryOperation_Plus(Word lhs, Word rhs)
{
	using namespace mix;

	if (Word::IsNegativeZero(lhs) && Word::IsZero(rhs))
	{
		// -0 +  0	== -0
		// -0 + -0	== -0
		// Note: 0 + -0	== 0
		return Word(WordValue(Sign::Negative, 0));
	}
	return Word(lhs.value() + rhs.value());
}

Word BinaryOperation_Minus(Word lhs, Word rhs)
{
	using namespace mix;

	if (Word::IsNegativeZero(lhs) && Word::IsZero(rhs))
	{
		// -0 -  0	== -0
		// -0 - -0	== -0
		// Note: 0 - -0	== 0
		return Word(WordValue(Sign::Negative, 0));
	}
	return Word(lhs.value() - rhs.value());
}

Word BinaryOperation_Multiply(Word lhs, Word rhs)
{
	return Word(lhs.value() * rhs.value());
}

Word BinaryOperation_Divide(Word lhs, Word rhs)
{
	if (Word::IsZero(rhs))
	{
		throw DivisionByZero{};
	}

	return Word(lhs.value() / rhs.value());
}

Word BinaryOperation_DoubleDivide(Word lhs, Word rhs)
{
	using namespace mix;

	if (Word::IsZero(rhs))
	{
		throw DivisionByZero{};
	}

	std::uint64_t v = lhs.abs_value();
	v <<= Word::k_bits_count;
	const auto abs_result = v / rhs.abs_value();
	const auto sign = (lhs.sign() == rhs.sign()) ? Sign::Positive : Sign::Negative;

	return Word(WordValue(sign, static_cast<int>(abs_result)));
}

Word BinaryOperation_Field(Word lhs, Word rhs)
{
	// Note: in theory, we can use `mix::WordField` to do calculations below
	// (to avoid duplications), but since values can be negative - this will not
	// work (because `WordField` works with non-negative values only)
	return Word(8 * lhs.value() + rhs.value());
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
	{"//", &BinaryOperation_DoubleDivide},
	{":", &BinaryOperation_Field},
};

} // namespace


Word CalculateUnaryOperation(const UnaryOperation& op, Word value)
{
	for (auto op_handler : k_unary_operations)
	{
		if (op_handler.op == op)
		{
			return op_handler.handler(std::move(value));
		}
	}

	throw UnknownUnaryOperation{op};
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

Word CalculateOptionalUnaryOperation(const std::optional<UnaryOperation>& op, Word value)
{
	if (!op)
	{
		return value;
	}

	return CalculateUnaryOperation(*op, std::move(value));
}

} // namespace mixal


