#pragma once
#include <mixal/parser.h>

namespace mixal {

class MIXAL_PARSER_LIB_EXPORT AddressParser final :
	public IParser
{
private:
	virtual std::size_t do_parse_stream(std::string_view str, std::size_t offset) override;
	virtual void do_clear() override;
};

} // namespace mixal
