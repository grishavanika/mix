#include <mixal_formatter/stream_formatter.h>

#include <iostream>

using namespace mixal_formatter;

int main()
{
	FormatOptions options;
	FormatStream(std::cin, std::cout, options);
}
