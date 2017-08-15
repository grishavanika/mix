#include <mixal/line_parser.h>

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
	return !str.empty() &&
		(str.front() == k_comment_begin_char);
}

} // namespace

///////////////////////////////////////////////////////////////////////////////
// #TODO: change to stream-based parsing !

// Note: implementation of this function relies on what is
// LABEL and OPERATION (e.g., strings that can not contain spaces)
//
// The idea is next:
// 
// Given MIXAL line of code: `LABEL OPERATION ADDRESS comment`,
// find `OPERATION` column and assume that
// everything on the left is `LABEL` and on the right - is `ADDRESS`
// (`LABEL` can be optional, comment should start with any lower-case char)
std::size_t LineParser::do_parse_stream(std::string_view str, std::size_t /*offset*/)
{
	auto line = core::LeftTrim(str);
	if (IsCommentBeginning(line))
	{
		comment_ = line;
		return str.size();
	}

	auto first_word_end = line.find_first_of(k_whitespaces);
	if (IsInvalidStreamPosition(first_word_end))
	{
		// All line is some word. The only valid situation is that it's Operation
		OperationParser op_parser;
		auto r = op_parser.parse_stream(line);
		assert(!IsInvalidStreamPosition(r));
		(void)r;
		operation_ = std::move(op_parser);
		return str.size();
	}

	OperationParser op_parser;
	auto label_or_op = line.substr(0, first_word_end);
	if (op_parser.parse_stream(label_or_op) == label_or_op.size())
	{
		// Line has NO LABEL
		parse_address_str_with_comment(line.substr(first_word_end));
		operation_ = std::move(op_parser);
		return str.size();
	}

	LabelParser label_parser;
	auto r = label_parser.parse_stream(label_or_op);
	assert(!IsInvalidStreamPosition(r));
	(void)r;

	auto second_word_begin = line.find_first_not_of(k_whitespaces, first_word_end + 1);
	if (IsInvalidStreamPosition(second_word_begin))
	{
		// Line contains only LABEL without OPERATION and ADDRESS
		return InvalidStreamPosition();
	}

	// Rest of line contains OPERATION and (possibly) ADDRESS with comment
	auto second_word_end = line.find_first_of(k_whitespaces, second_word_begin + 1);
	if (IsInvalidStreamPosition(second_word_end))
	{
		second_word_end = line.size();
	}

	r = op_parser.parse_stream(line.substr(second_word_begin, second_word_end - second_word_begin));
	assert(!IsInvalidStreamPosition(r));
	(void)r;

	operation_ = std::move(op_parser);
	label_ = std::move(label_parser);

	// ... and try to split ADDRESS from comment (if any)
	parse_address_str_with_comment(line.substr(second_word_end));
	return str.size();
}

void LineParser::parse_address_str_with_comment(std::string_view line)
{
	line = core::LeftTrim(line);

	const auto comment_start = core::FindIf(line, &islower);

	address_str_ = core::RightTrim(line.substr(0, comment_start));
	
	if (!IsInvalidStreamPosition(comment_start))
	{
		comment_ = line.substr(comment_start);
	}
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
	return comment_ && IsCommentBeginning(*comment_);
}

bool LineParser::has_inline_comment() const
{
	return comment_ && !IsCommentBeginning(*comment_);
}

