#pragma once
#include <mixal/parser.h>

namespace mixal {

class LabelParser final :
	public IParser
{
public:
	void parse(std::string_view str) override;
};

} // namespace mixal

