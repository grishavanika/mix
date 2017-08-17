#include <mixal_format/stream_formatter.h>

#include <algorithm>
#include <iterator>
#include <iostream>

#include <cassert>
#include <cctype>

namespace mixal_format {

namespace {

std::string ToUpper(const std::string& str)
{
	std::string all_upper_case;
	all_upper_case.reserve(str.size());
	std::transform(str.cbegin(), str.cend(),
		std::back_inserter(all_upper_case),
		[](char ch)
	{
		return static_cast<char>(std::toupper(ch));
	});
	return all_upper_case;
}

} // namespace

void FormatStream(std::istream& in, std::ostream& out, const FormatOptions& options /*= {}*/)
{
	std::string line;
	while (getline(in, line))
	{
		// #TODO: making uppercase should be controlled via flag
		out << FormatLine(ToUpper(line), options) << "\n";
		assert(out);
	}
}

} // namespace mixal_format
