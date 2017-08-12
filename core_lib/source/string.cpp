#include <core/string.h>

#include <algorithm>

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

	length = vsnprintf(
		StringWriteInto(str, length + 1),
		length + 1,
		format,
		args);

	va_end(args);


	str.resize(length);
	return str;
}

std::string_view LeftTrim(std::string_view str)
{
	auto first_not_space = find_if_not(str.begin(), str.end(), &std::isspace);
	if (first_not_space != str.end())
	{
		str.remove_prefix(distance(str.begin(), first_not_space));
		return str;
	}
	return {};
}

std::string_view RightTrim(std::string_view str)
{
	auto last_not_space = find_if_not(str.rbegin(), str.rend(), &std::isspace);
	if (last_not_space != str.rend())
	{
		str.remove_suffix(distance(str.rbegin(), last_not_space));
		return str;
	}
	return {};
}

std::string_view Trim(std::string_view str)
{
	return LeftTrim(RightTrim(str));
}

} // namespace core
