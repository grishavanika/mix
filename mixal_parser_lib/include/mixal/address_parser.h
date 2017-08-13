#pragma once
#include <mixal/parser.h>

namespace mixal {

class AddressParser final :
	public IParser
{
private:
	virtual void do_parse(std::string_view str) override;
	virtual void do_clear() override;
};

} // namespace mixal
