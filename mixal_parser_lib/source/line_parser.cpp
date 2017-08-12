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

	// The idea is next:
	// 
	// Given MIXAL line of code: `LABEL OPERATION ADDRESS`,
	// find `OPERATION` column and assume that
	// everything on the left is `LABEL` and on the right - is `ADDRESS`
	// (`LABEL` can be optional)

	auto line = core::LeftTrim(str);
	auto first_word_end = line.find(' ');
	if (first_word_end == line.npos)
	{
		// All line is some word. The only valid situation is that it's Operation
		OperationParser op_parser;
		op_parser.parse(line);
		operation_ = std::move(op_parser);
		return;
	}

	OperationParser op_parser;
	std::string_view label_or_operation{line.data(), first_word_end};
	if (op_parser.try_parse(label_or_operation))
	{
		std::string_view address_str{line.data() + first_word_end, line.size() - first_word_end};
		AddressParser address_parser;
		address_parser.parse(address_str);
		operation_ = std::move(op_parser);
		address_ = std::move(address_parser);
		return;
	}

	LabelParser label_parser;
	label_parser.parse(label_or_operation);

	auto second_word_begin = line.find_first_not_of(' ', first_word_end + 1);
	if (second_word_begin == line.npos)
	{
		// Line contains only LABEL without OPERATION and ADDRESS
		throw InvalidLine{};
	}

	auto second_word_end = line.find(' ', second_word_begin + 1);
	if (second_word_end == line.npos)
	{
		second_word_end = line.size();
	}

	std::string_view operation_str{line.data() + second_word_begin, second_word_end - second_word_begin};
	op_parser.parse(operation_str);

	const auto address_str_begin = second_word_begin + operation_str.size();
	std::string_view address_str{line.data() + address_str_begin, line.size() - address_str_begin};
	AddressParser address_parser;
	address_parser.parse(address_str);

	address_ = std::move(address_parser);
	operation_ = std::move(op_parser);
	label_ = std::move(label_parser);
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
