#pragma once
#include <core/assert.h>

#include <string>

#include <cstdarg>
#include <cstdio>
#include <cwchar>

namespace note {

namespace internal {

template<class StringType>
typename StringType::value_type* StringWriteInto(
	StringType& str, std::size_t length_with_null)
{
	assert(length_with_null > 1 && "Minimum length with null should be 2");
	if(length_with_null <= 1)
	{
		return nullptr;
	}

	str.reserve(length_with_null);
	str.resize(length_with_null - 1);
	return &str[0];
}

#pragma warning(push)
// This function or variable may be unsafe
#pragma warning(disable:4996)
// Note: "safe" functions do not support `nullptr` as first
// arguments (trick for getting needed buffer's size)

inline int StringNPrintF(char* buffer, size_t buffer_count,
	const char* format, va_list arg_list)
{
	return _vsnprintf(buffer, buffer_count, format, arg_list);
}

inline int StringNPrintF(wchar_t* buffer, size_t buffer_count,
	const wchar_t* format, va_list arg_list)
{
	return _vsnwprintf(buffer, buffer_count, format, arg_list);
}
#pragma warning(pop)

} // namespace internal

template<typename Elem>
std::basic_string<Elem> Sprintf(const Elem* format, ...)
{
	using namespace internal;
	std::basic_string<Elem> ret;
	
	va_list args;
	va_start(args, format);

	int length = StringNPrintF(nullptr, 0, format, args);
	assert(length >= 0);
	if(length <= 0)
	{
        return ret;
	}

	length = StringNPrintF(
		StringWriteInto(ret, length + 1),
	  	length + 1,
		format,
		args);
	assert(length == static_cast<int>(ret.size()));

	va_end(args);

	return ret;
}

std::string UTF16ToUTF8(const std::wstring& utf16);
std::wstring UTF8ToUTF16(const std::string& utf8);

} // namespace note

