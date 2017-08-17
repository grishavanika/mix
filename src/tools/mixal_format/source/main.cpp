#include <mixal_format/stream_formatter.h>

#include <iostream>

using namespace mixal_format;

int main()
{
	FormatOptions options;
	FormatStream(std::cin, std::cout, options);
}
