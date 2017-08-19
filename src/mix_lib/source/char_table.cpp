#include <mix/char_table.h>

#include <core/utils.h>

#include <algorithm>

#include <cassert>

namespace {

// Values for delta (0xeb), pi (0xe3), sigma (0xe4),
// (c) (0xb8) are taken from Extended ASCII table for
// given symbols
const char k_chars_table[] =
{
	' ', 'A', 'B', 'C', 'D', 'E', 'F', 'G',
	'H', 'I', '\xeb', 'J', 'K', 'L', 'M', 'N',
	'O', 'P', 'Q', 'R', '\xe4', '\xe3', 'S', 'T',
	'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1',
	'2', '3', '4', '5', '6', '7', '8', '9',
	'.', ',', '(', ')', '+', '-', '*', '/',
	'=', '$', '<', '>', '\xb8', ';', ':', '\'',
	//'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x',
};

void SetOptionalFlag(bool* flag, bool value)
{
	if (flag)
	{
		*flag = value;
	}
}

} // namespace

namespace mix {

char ByteToChar(Byte b, bool* converted /*= nullptr*/)
{
	const auto index = b.cast_to<std::size_t>();
	if (index < core::ArraySize(k_chars_table))
	{
		SetOptionalFlag(converted, true);
		return k_chars_table[index];
	}
	
	SetOptionalFlag(converted, false);
	return '\0';
}

Byte CharToByte(char ch, bool* converted /*= nullptr*/)
{
	// Can be replaced to table lookup also
	const auto begin = std::cbegin(k_chars_table);
	const auto end = std::cend(k_chars_table);
	const auto it = std::find(begin, end, ch);
	if (it != end)
	{
		SetOptionalFlag(converted, true);
		return std::distance(begin, it);
	}
	
	SetOptionalFlag(converted, false);
	return Byte::Max();
}

std::size_t ByteToDecimalDigit(const Byte& byte)
{
	const auto digit = (byte.cast_to<std::size_t>() % 10);
	assert(digit <= 9);
	return digit;
}

Byte DecimalDigitToByte(std::size_t digit)
{
	assert(digit <= 9);
	return CharToByte('0').cast_to<std::size_t>() + digit;
}


} // namespace mix

