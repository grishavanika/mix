#include <mixal_parse/label_parser.h>

#include <core/string.h>

#include <cctype>

using namespace mixal_parse;

namespace {

bool IsAllowedLabelLocalSymbol(const Symbol& s)
{
	return (s.kind() == LocalSymbolKind::Here) ||
		(s.kind() == LocalSymbolKind::Usual);
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

	const auto symbol = Symbol::FromString(name);
	if (!symbol.is_valid() ||
		!IsAllowedLabelLocalSymbol(symbol))
	{
		return InvalidStreamPosition();
	}

	label_ = symbol;
	return index;
}

void LabelParser::do_clear()
{
	label_ = {};
}

const Label& LabelParser::label() const
{
	return label_;
}
