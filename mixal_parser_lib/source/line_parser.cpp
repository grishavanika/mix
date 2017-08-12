#include <mixal/line_parser.h>
#include <mixal/parse_exceptions.h>

#include <core/string.h>

#include <cassert>

using namespace mixal;

namespace {

const char k_comment_begin_char = '*';

bool IsCommentBeginning(std::string_view str)
{
	auto without_first_whitespaces = core::LeftTrim(str);
	return !without_first_whitespaces.empty() &&
		(without_first_whitespaces.front() == '*');
}

} // namespace

void LineParser::parse(std::string_view str)
{
	clear();

	if (IsCommentBeginning(str))
	{
		comment_ = core::LeftTrim(str);
		return;
	}

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

std::string_view LineParser::str() const
{
	throw NotImplemented{};
}

void LineParser::clear()
{
	comment_.reset();
	label_.reset();
	operation_.reset();
	address_.reset();
}
