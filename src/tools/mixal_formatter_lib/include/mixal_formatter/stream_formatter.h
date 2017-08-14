#pragma once
#include <mixal_formatter/line_formatter.h>

#include <iosfwd>

namespace mixal_formatter {

void FormatStream(std::istream& in, std::ostream& out, const FormatOptions& options = {});

} // namespace mixal_formatter


