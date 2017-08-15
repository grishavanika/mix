#include <mixal/address_parser.h>
#include <mixal/parsers_utils.h>

#include <core/string.h>

using namespace mixal;

std::size_t AddressParser::do_parse_stream(std::string_view str, std::size_t offset)
{
	const auto first_char_pos = SkipLeftWhiteSpaces(str, offset);
	if (first_char_pos == str.size())
	{
		// Empty `Address` is valid
		return str.size();
	}

	return InvalidStreamPosition();
}

void AddressParser::do_clear()
{
}
