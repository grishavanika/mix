#pragma once
#include <mixal/parser.h>

namespace mixal {

class AddressParser final :
	public IParser
{
public:
	virtual void parse(std::string_view str) override;
	virtual std::string_view str() const override;

};

} // namespace mixal
