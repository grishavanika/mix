#include <mix/char_table.h>

#include <algorithm>

namespace {

	const int k_chars_table[] =
	{
		' ', 'A', 'B', 'C', 'D', 'E', 'F', 'G',
		'H', 'I', '\xeb', 'J', 'K', 'L', 'M', 'N',
		'O', 'P', 'Q', 'R', '\xe4', '0xe3', 'S', 'T',
		'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1',
		'2', '3', '4', '5', '6', '7', '8', '9',
		'.', ',', '(', ')', '+', '-', '*', '/',
		'=', '$', '<', '>', '\xb8', ';', ':', '\'',
		'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x',
	};

	static_assert((sizeof(k_chars_table) / sizeof(int)) == mix::Byte::k_values_count,
		"Chars table should cover all possible Byte values");

} // namespace

namespace mix {

Char ByteToChar(Byte b)
{
	return static_cast<Char>(k_chars_table[b.cast_to<std::size_t>()]);
}

Byte CharToByte(Char ch)
{
	// Can be replaced to table lookup also
	const auto begin = std::cbegin(k_chars_table);
	const auto end = std::cend(k_chars_table);
	const auto it = std::find(begin, end, static_cast<int>(ch));
	if (it != std::cend(k_chars_table))
	{
		return Byte{std::distance(begin, it)};
	}
	
	return Byte::Max();
}

} // namespace mix

