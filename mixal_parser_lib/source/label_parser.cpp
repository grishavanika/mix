#include <mixal/label_parser.h>
#include <mixal/parse_exceptions.h>

using namespace mixal;

void LabelParser::parse(std::string_view /*str*/)
{
	throw NotImplemented{};
}

std::string_view LabelParser::str() const
{
	throw NotImplemented{};
}

std::string_view LabelParser::name() const
{
	return str();
}

bool LabelParser::is_local_symbol() const
{
	return !!local_symbol_id_;
}

std::optional<LocalSymbolId> LabelParser::local_symbol_id() const
{
	return local_symbol_id_;
}
