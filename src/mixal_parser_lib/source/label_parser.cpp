#include <mixal/label_parser.h>

#include <core/string.h>

#include <sstream>

#include <cassert>
#include <cctype>

using namespace mixal;

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
	LocalSymbolId n = 0;
	in >> n;
	assert(in);

	return n;
}

bool IsValidLabelLocalSymbol(const std::string_view& str)
{
	assert(IsLocalSymbol(str));
	return (str.back() == k_local_symbol_label_suffix);
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

	name_ = str.substr(first_non_space, index - first_non_space);

	if (!IsSymbol(name_))
	{
		return InvalidStreamPosition();
	}

	if (IsLocalSymbol(name_) &&
		!parse_local_symbol())
	{
		return InvalidStreamPosition();
	}

	return index;
}

bool LabelParser::parse_local_symbol()
{
	if (!IsValidLabelLocalSymbol(name_))
	{
		return false;
	}

	local_symbol_id_ = ParseLocalSymbol(name_);
	return IsValidLocalSymbolId(*local_symbol_id_);
}

std::string_view LabelParser::name() const
{
	return name_;
}

bool LabelParser::is_local_symbol() const
{
	return !!local_symbol_id_;
}

std::optional<LocalSymbolId> LabelParser::local_symbol_id() const
{
	return local_symbol_id_;
}

bool LabelParser::empty() const
{
	return name_.empty();
}

void LabelParser::do_clear()
{
	local_symbol_id_ = std::nullopt;
	name_ = {};
}
