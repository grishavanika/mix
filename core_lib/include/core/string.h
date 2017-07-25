#pragma once
#include <string>

namespace core {

char* StringWriteInto(std::string& str, std::size_t length_with_null);

std::string Sprintf(const char* format, ...);

} // namespace core

