#include <mixal_formatter/stream_formatter.h>

#include <iostream>

#include <cassert>

namespace mixal_formatter {

void FormatStream(std::istream& in, std::ostream& out, const FormatOptions& options /*= {}*/)
{
	std::string line;
	while (getline(in, line))
	{
		out << FormatLine(line, options) << "\n";
		assert(out);
	}
}

} // namespace mixal_formatter
