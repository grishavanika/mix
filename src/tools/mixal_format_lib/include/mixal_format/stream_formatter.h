#pragma once
#include <mixal_format/line_formatter.h>

#include <iosfwd>

namespace mixal_format {

MIXAL_FORMAT_LIB_EXPORT
void FormatStream(std::istream& in, std::ostream& out, const FormatOptions& options = {});

} // namespace mixal_format


