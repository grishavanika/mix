#include <mixal_parse/label_parser.h>

#include <core/string.h>

#include <sstream>

#include <cassert>
#include <cctype>

using namespace mixal_parse;

namespace {

const char k_local_symbol_label_suffix = 'H';
const std::string_view k_local_symbols = "BHF";

bool IsLocalSymbol(const std::string_view& str)
{
	assert(!str.empty());

	if (k_local_symbols.find(str.back()) != std::string_view::npos)
	{
		return IsNumber(str.substr(0, str.size() - 1));
	}

	return false;
}

LocalSymbolId ParseLocalSymbol(const std::string_view& str)
{
	assert(IsLocalSymbol(str));

	std::stringstream in;
	in << str.substr(0, str.size() - 1);
	LocalSymbolId n = k_invalid_local_symbol_id;
	in >> n;
	assert(in);

	return n;
}

bool IsAllowedLabelLocalSymbol(const std::string_view& str)
{
	if (IsLocalSymbol(str))
	{
		return (str.back() == k_local_symbol_label_suffix);
	}
	return false;
}

} // namespace

std::size_t LabelParser::do_parse_stream(std::string_view str, std::size_t offset)
{
	const auto first_non_space = SkipLeftWhiteSpaces(str, offset);
	if (first_non_space == str.size())
	{
		return str.size();
	}

	auto index = first_non_space;
	for (auto size = str.size(); index < size; ++index)
	{
		if (!IsSymbolChar(str[index]))
		{
			break;
		}
	}

	const auto name = str.substr(first_non_space, index - first_non_space);

	if (!IsSymbol(name))
	{
		return InvalidStreamPosition();
	}

	LocalSymbolId local_id = k_invalid_local_symbol_id;
	if (IsLocalSymbol(name))
	{
		local_id = parse_local_symbol(name);
		if (!IsValidLocalSymbolId(local_id))
		{
			return InvalidStreamPosition();
		}
	}

	label_ = Label{Symbol{name}, local_id};
	return index;
}

LocalSymbolId LabelParser::parse_local_symbol(const std::string_view& str)
{
	if (!IsAllowedLabelLocalSymbol(str))
	{
		return k_invalid_local_symbol_id;
	}

	return ParseLocalSymbol(str);
}

void LabelParser::do_clear()
{
	label_ = {};
}

const Label& LabelParser::label() const
{
	return label_;
}
