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
	if (!options.title_comment.empty())
	{
		out << "* " << options.title_comment << "\n";
	}

	std::string line;
	while (getline(in, line))
	{
		line = options.make_all_uppercase ? ToUpper(line) : line;
		
		const auto formatted_line = FormatLine(line, options);
		if (formatted_line.empty() && options.mdk_compatible)
		{
			continue;
		}

		out << formatted_line << '\n';
		assert(out);
	}
}

} // namespace mixal_format
