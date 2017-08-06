#pragma once
#include <mixal/parser.h>

namespace mixal {

class OperationParser final :
	public IParser
{
public:
	void parse(std::string_view str) override;
};

} // namespace mixal
