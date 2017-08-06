#include <mixal/line_parser.h>
#include <mixal/parse_exceptions.h>

using namespace mixal;

void LineParser::parse(std::string_view /*str*/)
{
	throw NotImplemented{};
}

const std::string_view* LineParser::comment() const
{
	return comment_ ? &*comment_ : nullptr;
}

const LabelParser* LineParser::label() const
{
	return label_ ? &*label_: nullptr;
}

const OperationParser* LineParser::operation() const
{
	return operation_ ? &*operation_ : nullptr;
}

const AddressParser* LineParser::address() const
{
	return address_ ? &*address_ : nullptr;
}
