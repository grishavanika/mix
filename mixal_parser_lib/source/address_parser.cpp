#include <mixal/address_parser.h>
#include <mixal/parse_exceptions.h>

#include <core/string.h>

using namespace mixal;

void AddressParser::do_parse(std::string_view str)
{
	auto address_expr = core::Trim(str);
	if (address_expr.empty())
	{
		return;
	}

	throw NotImplemented{};
}

void AddressParser::do_clear()
{
}
