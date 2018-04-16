#pragma once
#include <string>

namespace mixal_format {

struct FormatOptions
{
	bool mdk_compatible = false;
	bool make_all_uppercase = true;
	std::string title_comment;
};

std::string FormatLine(const std::string& line, const FormatOptions& options = {});

} // namespace mixal_format
