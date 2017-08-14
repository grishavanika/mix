#pragma once
#include <string>
#include <string_view>
#include <vector>

namespace core {

char* StringWriteInto(std::string& str, std::size_t length_with_null);

std::string Sprintf(const char* format, ...);

std::string_view LeftTrim(std::string_view str);
std::string_view RightTrim(std::string_view str);
std::string_view Trim(std::string_view str);

std::vector<std::string_view> Split(std::string_view str, char ch);

} // namespace core

