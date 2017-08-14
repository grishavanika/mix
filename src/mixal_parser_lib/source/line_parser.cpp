#include <mixal/line_parser.h>
#include <mixal/parse_exceptions.h>

#include <core/string.h>

#include <algorithm>

#include <cassert>
#include <cctype>

using namespace mixal;

namespace {

const char k_comment_begin_char = '*';
const char k_whitespaces[] = " \t";

bool IsCommentBeginning(std::string_view str)
{
	auto without_first_whitespaces = core::LeftTrim(str);
	return !without_first_whitespaces.empty() &&
		(without_first_whitespaces.front() == k_comment_begin_char);
}

} // namespace

// Note: implementation of this function relies on what is
// LABEL and OPERATION (e.h., strings that can not contain spaces)
void LineParser::do_parse(std::string_view str)
{
	if (IsCommentBeginning(str))
	{
		comment_ = core::LeftTrim(str);
		return;
	}

	// The idea is next:
	// 
	// Given MIXAL line of code: `LABEL OPERATION ADDRESS comment`,
	// find `OPERATION` column and assume that
	// everything on the left is `LABEL` and on the right - is `ADDRESS`
	// (`LABEL` can be optional, comment should start with any lower-case char)

	auto line = core::LeftTrim(str);
	auto first_word_end = line.find_first_of(k_whitespaces);
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
		// Line has NO LABEL
		parse_address_str_with_comment({line.data() + first_word_end, line.size() - first_word_end});
		operation_ = std::move(op_parser);
		return;
	}

	LabelParser label_parser;
	label_parser.parse(label_or_operation);

	auto second_word_begin = line.find_first_not_of(k_whitespaces, first_word_end + 1);
	if (second_word_begin == line.npos)
	{
		// Line contains only LABEL without OPERATION and ADDRESS
		throw InvalidLine{};
	}

	// Rest of line contains OPERATION and (possibly) ADDRESS with comment
	auto second_word_end = line.find_first_of(k_whitespaces, second_word_begin + 1);
	if (second_word_end == line.npos)
	{
		second_word_end = line.size();
	}

	std::string_view operation_str{line.data() + second_word_begin, second_word_end - second_word_begin};
	op_parser.parse(operation_str);
	
	operation_ = std::move(op_parser);
	label_ = std::move(label_parser);

	// ... and try to split ADDRESS from comment (if any)
	const auto address_str_begin = second_word_begin + operation_str.size();
	std::string_view address_with_comment{line.data() + address_str_begin, line.size() - address_str_begin};
	parse_address_str_with_comment(address_with_comment);
}

void LineParser::parse_address_str_with_comment(std::string_view line)
{
	line = core::LeftTrim(line);

	auto comment_start_it = std::find_if(line.begin(), line.end(),
		[](char ch)
		{
			return std::islower(ch);
		});

	if (comment_start_it == line.end())
	{
		address_str_ = line;
		return;
	}

	const auto address_str_end = std::distance(line.begin(), comment_start_it);

	address_str_ = core::RightTrim({line.data(), static_cast<std::size_t>(address_str_end)});
	comment_ = {line.data() + address_str_end, line.size() - address_str_end};
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

std::string_view LineParser::address_str() const
{
	return address_str_;
}

void LineParser::do_clear()
{
	comment_ = std::nullopt;
	label_ = std::nullopt;
	operation_ = std::nullopt;
	address_str_ = {};
}

bool LineParser::has_only_comment() const
{
	return comment_ ? (comment_->front() == k_comment_begin_char) : false;
}

bool LineParser::has_inline_comment() const
{
	return comment_ ? (comment_->front() != k_comment_begin_char) : false;
}

