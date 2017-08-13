#include <mixal/parsers_utils.h>

#include <algorithm>

#include <cctype>

namespace mixal {

extern const std::size_t k_max_symbol_length = 10;
extern const char k_current_address_marker = '*';

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

	bool has_alpha_char = false;
	for (auto symbol : str)
	{
		if (std::isdigit(symbol))
		{
			continue;
		}

		if (!IsMixAlphaCharacter(symbol))
		{
			return false;
		}

		has_alpha_char = true;
	}

	return has_alpha_char;
}

bool IsNumber(const std::string_view& str)
{
	if (str.size() > k_max_symbol_length)
	{
		return false;
	}

	return std::all_of(str.cbegin(), str.cend(), &std::isdigit);
}

bool IsCurrentAddressSymbol(const std::string_view& str)
{
	return (str.size() == 1) &&
		(str.front() == k_current_address_marker);
}

bool IsBasicExpression(const std::string_view& str)
{
	// #NOTE: instead of "any" symbol, basic expression needs
	// "already defined" symbol (not "forward-reference" symbol)
	return IsSymbol(str) || IsNumber(str) || IsCurrentAddressSymbol(str);
}

} // namespace mixal
