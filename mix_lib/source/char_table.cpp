#include <mix/char_table.h>

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
		'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x',
	};

	static_assert(sizeof(k_chars_table) == mix::Byte::k_values_count,
		"Chars table should cover all possible Byte values");

} // namespace

namespace mix {

char ByteToChar(Byte b)
{
	return k_chars_table[b.cast_to<std::size_t>()];
}

Byte CharToByte(char ch)
{
	// Can be replaced to table lookup also
	const auto begin = std::cbegin(k_chars_table);
	const auto end = std::cend(k_chars_table);
	const auto it = std::find(begin, end, ch);
	if (it != end)
	{
		return Byte{std::distance(begin, it)};
	}
	
	return Byte::Max();
}

std::size_t ByteToDecimalDigit(const Byte& byte)
{
	const auto digit = (byte.cast_to<std::size_t>() % 10);
	assert((digit >= 0) && (digit <= 9));
	return digit;
}

Byte DecimalDigitToByte(std::size_t digit)
{
	assert((digit >= 0) && (digit <= 9));
	return Byte{CharToByte('0').cast_to<std::size_t>() + digit};
}


} // namespace mix

