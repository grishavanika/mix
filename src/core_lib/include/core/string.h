#pragma once
#include <core/config.h>

#include <string>

#if defined(_MSC_VER) && !defined(__clang__)
#  include <string_view>
#elif __has_include(<string_view>)
#  include <string_view>
#elif __has_include(<experimental/string_view>)
#  include <experimental/string_view>

namespace std {
using string_view = experimental::string_view;
} // namespace std
#endif

#include <vector>
#include <algorithm>

namespace core {

CORE_LIB_EXPORT
char* StringWriteInto(std::string& str, std::size_t length_with_null);

CORE_LIB_EXPORT
std::string Sprintf(const char* format, ...);

CORE_LIB_EXPORT
std::string_view LeftTrim(std::string_view str);

CORE_LIB_EXPORT
std::string_view RightTrim(std::string_view str);

CORE_LIB_EXPORT
std::string_view Trim(std::string_view str);

CORE_LIB_EXPORT
std::vector<std::string_view> Split(std::string_view str, char ch);

template<typename Pred>
std::size_t FindIf(const std::string_view& str, Pred p)
{
	const auto begin = str.cbegin();
	const auto end = str.cend();
	const auto it = std::find_if(begin, end, std::move(p));

	return (it == end)
		? std::string_view::npos
		: static_cast<std::size_t>(std::distance(begin, it));
}

} // namespace core

