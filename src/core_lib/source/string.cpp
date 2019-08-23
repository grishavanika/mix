#include <core/string.h>

#include <algorithm>
#include <functional>

#include <cstdio>
#include <cstdarg>
#include <cassert>
#include <cctype>

namespace core {

// From somewhere inside chrome...
char* StringWriteInto(std::string& str, std::size_t length_with_null)
{
	if (length_with_null <= 1)
	{
		return nullptr;
	}

	str.reserve(length_with_null);
	str.resize(length_with_null - 1);
	return &str[0];
}

std::string Sprintf(const char* format, ...)
{
	std::string str;

	va_list args;
	va_start(args, format);

#if defined(_MSC_VER)
#  pragma warning(push)
	// This function or variable may be unsafe
#  pragma warning(disable:4996)
	int length = _vsnprintf(nullptr, 0, format, args);
#  pragma warning(pop)
#else
	int length = 1 * 1024;
#endif

	if (length <= 0)
	{
		return str;
	}

#if defined(__clang__)
#  pragma clang diagnostic push
	// format string is not a string literal
#  pragma clang diagnostic ignored "-Wformat-nonliteral"
#endif
    const std::size_t count = static_cast<std::size_t>(length) + 1;
	length = vsnprintf(
		StringWriteInto(str, count),
		count,
		format,
		args);

#if defined(__clang__)
#  pragma clang diagnostic pop
#endif

	va_end(args);


	str.resize(length);
	return str;
}

std::string_view LeftTrim(std::string_view str)
{
	auto first_not_space = FindIf(str, [](char ch)
	{
		return !std::isspace(ch);
	});

	if (first_not_space != std::string_view::npos)
	{
		str.remove_prefix(first_not_space);
		return str;
	}
	return {};
}

std::string_view RightTrim(std::string_view str)
{
	auto last_not_space = find_if_not(str.rbegin(), str.rend(),
		[](char ch)
		{
			return std::isspace(ch);
		});

	if (last_not_space != str.rend())
	{
		str.remove_suffix(std::distance(str.rbegin(), last_not_space));
		return str;
	}
	return {};
}

std::string_view Trim(std::string_view str)
{
	return LeftTrim(RightTrim(str));
}

std::vector<std::string_view> Split(std::string_view str, char ch)
{
	std::vector<std::string_view> parts;

	std::size_t prev_pos = 0;

	auto push_new_part = [&](std::size_t end)
	{
		parts.push_back(str.substr(prev_pos, end - prev_pos));
	};

	std::size_t pos = 0;

	auto find_next = [&]()
	{
		pos = str.find(ch, prev_pos);
		return pos;
	};

	while (find_next() != std::string_view::npos)
	{
		push_new_part(pos);
		prev_pos = pos + 1;
	}

	push_new_part(str.size());
	return parts;
}

} // namespace core
