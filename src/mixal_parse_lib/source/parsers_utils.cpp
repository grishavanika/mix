#include <mixal_parse/parsers_utils.h>

#include <sstream>
#include <algorithm>

#include <cctype>
#include <cassert>

namespace mixal_parse {

extern const std::size_t k_max_symbol_length = 10;
extern const std::size_t k_max_operation_str_length = 4;
extern const char k_current_address_marker = '*';
extern const LocalSymbolId k_max_local_symbol_id = 9;
extern const LocalSymbolId k_invalid_local_symbol_id = -1;

extern const UnaryOperation k_unary_operations[] =
{
	"-", "+",
};

extern const BinaryOperation k_binary_operations[] =
{
	"-", "+", "*", "/", "//", ":",
};

namespace {

const std::string_view k_local_symbols = "BHF";

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

	return !str.empty() &&
		std::all_of(str.cbegin(), str.cend(), &IsNumberChar);
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
		if (unary_op.find(ch) != std::string_view::npos)
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
		if (binary_op.find(ch) != std::string_view::npos)
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
		// Since Symbol can also start as Number,
		// need to check both: symbol & number validness
		return IsNumber(str) || IsSymbol(str);
	}
	else if (IsSymbolBegin(first_char))
	{
		return IsSymbol(str);
	}

	return false;
}

std::size_t SkipLeftWhiteSpaces(const std::string_view& str, std::size_t offset /*= 0*/)
{
	if (offset > str.size())
	{
		return str.size();
	}

	auto last_not_space = std::find_if_not(str.begin() + offset, str.end(), &isspace);
	if (last_not_space == str.end())
	{
		return str.size();
	}
	return static_cast<std::size_t>(std::distance(str.begin(), last_not_space));
}

std::size_t ExpectFirstNonWhiteSpaceChar(char ch, const std::string_view& str, std::size_t offset /*= 0*/)
{
	const auto first_char = SkipLeftWhiteSpaces(str, offset);
	if ((first_char == str.size()) || (str[first_char] != ch))
	{
		return str.size();
	}
	return first_char;
}

bool IsValidLocalSymbolId(LocalSymbolId id)
{
	return (id >= 0) && (id <= k_max_local_symbol_id);
}

bool IsLocalSymbol(const std::string_view& str)
{
	if (str.empty())
	{
		return false;
	}

	if (k_local_symbols.find(str.back()) != std::string_view::npos)
	{
		return IsNumber(str.substr(0, str.size() - 1));
	}

	return false;
}

LocalSymbolKind ParseLocalSymbolKind(const std::string_view& str)
{
	if (!IsLocalSymbol(str))
	{
		return LocalSymbolKind::Unknown;
	}

	switch (str.back())
	{
	case 'H': return LocalSymbolKind::Here;
	case 'F': return LocalSymbolKind::Forward;
	case 'B': return LocalSymbolKind::Backward;
	}

	return LocalSymbolKind::Unknown;
}

LocalSymbolId ParseLocalSymbolId(const std::string_view& str)
{
	if (!IsLocalSymbol(str))
	{
		return k_invalid_local_symbol_id;
	}

	LocalSymbolId n = k_invalid_local_symbol_id;

	// #TODO: C++17: `std::from_chars()`
	std::stringstream in;
	in << str.substr(0, str.size() - 1);
	in >> n;

	assert(in);
	return n;
}

std::size_t InvalidStreamPosition()
{
	return std::string_view::npos;
}

bool IsInvalidStreamPosition(std::size_t pos)
{
	return (pos == InvalidStreamPosition());
}

} // namespace mixal_parse
