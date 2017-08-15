#pragma once
#include <string>

#if defined(_MSC_VER) && !defined(__clang__)
#include <string_view>
#elif __has_include(<experimental/string_view>)
#include <experimental/string_view>

namespace std {
using string_view = experimental::string_view;
} // namespace std

#else
#include <string_view>
#endif

#include <vector>
#include <algorithm>

namespace core {

char* StringWriteInto(std::string& str, std::size_t length_with_null);

std::string Sprintf(const char* format, ...);

std::string_view LeftTrim(std::string_view str);
std::string_view RightTrim(std::string_view str);
std::string_view Trim(std::string_view str);

std::vector<std::string_view> Split(std::string_view str, char ch);

template<typename Pred>
std::size_t FindIf(std::string_view str, Pred p)
{
	const auto begin = str.cbegin();
	const auto end = str.cend();
	const auto it = std::find_if(begin, end, std::move(p));

	return (it == end)
		? str.npos
		: static_cast<std::size_t>(std::distance(begin, it));
}

} // namespace core

