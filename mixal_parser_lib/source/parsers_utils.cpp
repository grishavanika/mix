#include <mixal/parsers_utils.h>

#include <algorithm>

#include <cctype>
#include <cassert>

namespace mixal {

extern const std::size_t k_max_symbol_length = 10;
extern const char k_current_address_marker = '*';

extern const UnaryOperation k_unary_operations[] =
{
	"-", "+",
};

extern const BinaryOperation k_binary_operations[] =
{
	"-", "+", "*", "/", "//", ":",
};

namespace {

bool IsCurrentAddressChar(char ch)
{
	return (ch == k_current_address_marker);
}

} // namespace

bool IsMixAlphaCharacter(char symbol)
{
	// #TODO: get alpha-chars from MIX chars table
	return std::isalpha(symbol) && std::isupper(symbol);
}

bool IsSymbol(const std::string_view& str)
{
	if (str.size() > k_max_symbol_length)
	{
		return false;
	}

	bool has_at_least_one_char = false;
	for (auto symbol : str)
	{
		if (IsNumberChar(symbol))
		{
			continue;
		}

		if (!IsMixAlphaCharacter(symbol))
		{
			return false;
		}

		has_at_least_one_char = true;
	}

	return has_at_least_one_char;
}

bool IsNumber(const std::string_view& str)
{
	if (str.size() > k_max_symbol_length)
	{
		return false;
	}

	return std::all_of(str.cbegin(), str.cend(), &IsNumberChar);
}

bool IsCurrentAddressSymbol(const std::string_view& str)
{
	return (str.size() == 1) &&
		IsCurrentAddressChar(str.front());
}

bool IsBasicExpression(const std::string_view& str)
{
	// #NOTE: instead of "any" symbol, basic expression needs
	// "already defined" symbol (not "forward-reference" symbol)
	return IsSymbol(str) || IsNumber(str) || IsCurrentAddressSymbol(str);
}

bool IsUnaryOperationBegin(char ch)
{
	for (auto unary_op : k_unary_operations)
	{
		assert(!unary_op.empty());
		if (unary_op.front() == ch)
		{
			return true;
		}
	}

	return false;
}

bool IsNumberBegin(char ch)
{
	return IsNumberChar(ch);
}

bool IsSymbolBegin(char ch)
{
	return IsSymbolChar(ch);
}

bool IsBasicExpressionBegin(char ch)
{
	// IsSymbolBegin() covers IsNumberBegin()
	return IsSymbolBegin(ch) ||
		IsCurrentAddressChar(ch);
}

bool IsUnaryOperationChar(char ch)
{
	for (const auto& unary_op : k_unary_operations)
	{
		if (unary_op.find(ch) != unary_op.npos)
		{
			return true;
		}
	}
	return false;
}

bool IsBinaryOperationChar(char ch)
{
	for (const auto& binary_op : k_binary_operations)
	{
		if (binary_op.find(ch) != binary_op.npos)
		{
			return true;
		}
	}
	return false;
}

bool IsNumberChar(char ch)
{
	return std::isdigit(ch);
}

bool IsSymbolChar(char ch)
{
	return IsMixAlphaCharacter(ch) ||
		IsNumberChar(ch);
}

bool IsBasicExpressionChar(char ch)
{
	// IsSymbolChar() covers IsNumberChar()
	return IsCurrentAddressChar(ch) || IsSymbolChar(ch);
}

bool IsCompletedUnaryOperation(const std::string_view& str)
{
	auto it = find(begin(k_unary_operations), end(k_unary_operations), str);
	return (it != end(k_unary_operations));
}

bool IsCompletedBinaryOperation(const std::string_view& str)
{
	auto it = find(begin(k_binary_operations), end(k_binary_operations), str);
	return (it != end(k_binary_operations));
}

bool IsCompletedBasicExpression(const std::string_view& str)
{
	if (str.empty())
	{
		return false;
	}

	const char first_char = str.front();
	if (IsCurrentAddressChar(first_char))
	{
		return IsCurrentAddressSymbol(str);
	}
	else if (IsNumberBegin(first_char))
	{
		return IsNumber(str);
	}
	else if (IsSymbolBegin(first_char))
	{
		return IsSymbol(str);
	}

	return false;
}

} // namespace mixal
