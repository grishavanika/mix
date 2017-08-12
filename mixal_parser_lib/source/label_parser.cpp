#include <mixal/label_parser.h>
#include <mixal/parse_exceptions.h>

#include <core/string.h>

#include <cassert>
#include <cctype>

using namespace mixal;

namespace {

const char k_local_symbol_label_suffix = 'H';

bool IsMaybeLocalSymbol(std::string_view str)
{
	assert(!str.empty());
	// #TODO: `2F`, `3B` are also local symbols, but they should not be
	// in the "Label" column
	if (str.back() != k_local_symbol_label_suffix)
	{
		return false;
	}

	return (str.size() == 2) && std::isdigit(str.front());
}

LocalSymbolId ParseLocalSymbol(std::string_view str)
{
	assert(IsMaybeLocalSymbol(str));
	return (str.front() - '0');
}

bool IsMixAlphaCharacter(char symbol)
{
	// #TODO: get alpha-chars from MIX chars table
	return std::isalpha(symbol) && std::isupper(symbol);
}

bool IsValidLabel(std::string_view str)
{
	if (str.size() > LabelParser::k_max_label_length)
	{
		return false;
	}

	bool has_alpha_char = false;
	for (auto symbol : str)
	{
		if (std::isdigit(symbol))
		{
			continue;
		}

		if (!IsMixAlphaCharacter(symbol))
		{
			return false;
		}
		
		has_alpha_char = true;
	}

	return has_alpha_char;
}

} // namespace

void LabelParser::parse(std::string_view str)
{
	auto label = core::Trim(str);
	if (label.empty())
	{
		return;
	}

	if (IsMaybeLocalSymbol(str))
	{
		local_symbol_id_ = ParseLocalSymbol(label);
	}
	else if (!IsValidLabel(label))
	{
		throw InvalidLabel{};
	}

	name_ = label;
}

std::string_view LabelParser::str() const
{
	return name();
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
