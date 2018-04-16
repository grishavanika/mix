#pragma once
#include <mixal_format/config.h>
#include <string>

namespace mixal_format {

struct MIXAL_FORMAT_LIB_EXPORT FormatOptions
{
	bool mdk_compatible = false;
	bool make_all_uppercase = true;
	std::string title_comment;
};

MIXAL_FORMAT_LIB_EXPORT
std::string FormatLine(const std::string& line, const FormatOptions& options = {});

} // namespace mixal_format
