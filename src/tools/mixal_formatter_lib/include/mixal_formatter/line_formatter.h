#pragma once
#include <mixal_formatter/config.h>
#include <string>

namespace mixal_formatter {

struct MIXAL_FORMATTER_LIB_EXPORT FormatOptions
{
	
};

MIXAL_FORMATTER_LIB_EXPORT
std::string FormatLine(const std::string& line, const FormatOptions& options = {});

} // namespace mixal_formatter
