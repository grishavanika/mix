#include <mixal/label_parser.h>
#include <mixal/parse_exceptions.h>
#include <mixal/parsers_utils.h>

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

} // namespace

void LabelParser::do_parse(std::string_view str)
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
	else if (!IsSymbol(label))
	{
		throw InvalidLabel{};
	}

	name_ = label;
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
	local_symbol_id_.reset();
	name_ = {};
}
