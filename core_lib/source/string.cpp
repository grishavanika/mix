#include <core/string.h>

// #TODO: define NOMINMAX and LEAN_AND_MEAN with CMake
#include <Windows.h>

namespace note {
namespace {
bool WideStringToMultibyte(const std::wstring& wstr, std::string& str, UINT code_page)
{
	if(wstr.empty())
	{
		str.clear();
		return true;
	}

	auto input_length = static_cast<int>(wstr.size());
	int bytes_count = ::WideCharToMultiByte(
		code_page, 0, &wstr[0], input_length, nullptr, 0, nullptr, nullptr);
	if(bytes_count == 0)
	{
		return false;
	}

	int writed_bytes = ::WideCharToMultiByte(
		code_page, 0, &wstr[0], input_length,
		internal::StringWriteInto(str, bytes_count + 1),
		bytes_count, nullptr, nullptr);
	return (writed_bytes == bytes_count);
}

bool MultibyteStringToWide(const std::string& mstr,
	std::wstring& wstr, UINT code_page, DWORD flags = 0)
{
	if(mstr.empty())
	{
		wstr.clear();
		return true;
	}

	auto input_length = static_cast<int>(mstr.size());
	int length = ::MultiByteToWideChar(code_page, flags,
		&mstr[0], input_length, nullptr, 0);
	if(length == 0)
	{
		return false;
	}
	
	int writen_chars = ::MultiByteToWideChar(
		code_page, flags, &mstr[0], input_length,
		internal::StringWriteInto(wstr, length + 1), length);

	return (writen_chars == length);
}

} // namespace

std::string UTF16ToUTF8(const std::wstring& utf16)
{
	std::string utf8;
	bool ok = WideStringToMultibyte(utf16, utf8, CP_UTF8);
	assert(ok);
	return utf8;
}

std::wstring UTF8ToUTF16(const std::string& utf8)
{
	std::wstring utf16;
	bool ok = MultibyteStringToWide(utf8, utf16, CP_UTF8);
	assert(ok);
	return utf16;
}

} // namespace note
