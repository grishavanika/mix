#include <mixal_parse/line_parser.h>

#include <core/string.h>

#include <algorithm>

#include <cassert>
#include <cctype>

using namespace mixal_parse;

namespace {

const char k_comment_begin_char = '*';

bool IsCommentBeginning(const std::string_view& str)
{
	return !str.empty() &&
		(str.front() == k_comment_begin_char);
}

} // namespace

std::size_t LineParser::do_parse_stream(std::string_view str, std::size_t offset)
{
	const auto first_char_pos = SkipLeftWhiteSpaces(str, offset);
	if (first_char_pos == str.size())
	{
		return InvalidStreamPosition();
	}

	if (IsCommentBeginning(str.substr(first_char_pos)))
	{
		comment_ = str.substr(first_char_pos);
		return str.size();
	}

	auto pos = try_parse_line_without_label(str, first_char_pos);
	if (IsInvalidStreamPosition(pos))
	{
		pos = try_parse_line_with_label(str, first_char_pos);
	}
	
	return pos;
}

std::size_t LineParser::try_parse_line_without_label(
	const std::string_view& str,
	std::size_t offset,
	bool fallback_to_label_parse /*= true*/)
{
	// Basically, because we do not separate LABEL and OPERATION columns
	// with single space, but with multiple spaces, there are
	// more logic to determine what is LABEL and what is OPERATION.
	// We are trying to parse line like it starts with OPERATION and if we failed
	// then return and lets the caller tries to parse line that starts with LABEL

	OperationParser op_parser_of_label;
	const auto op_end = op_parser_of_label.parse_stream(str, offset);
	if (IsInvalidStreamPosition(op_end))
	{
		return InvalidStreamPosition();
	}

	if (fallback_to_label_parse)
	{
		// Ok, LINE starts with valid OPERATION, but it can be the name
		// of the LABEL (syntax does not disallow such behavior).
		// We are trying to parse next token as OPERATION also.
		// 
		// If it is OPERATION (e.g., we have next LINE: "OPERATION OPERATION"),
		// then choose the way where first OPERATION is LABEL and
		// return to the caller to parse line that starts with LABEL
		// 
		// (Small note: do this only if line has something after our
		// 2nd OPERATION, i.e., we have next line: "OPERATION OPERATION ADDRESS stuff")
		OperationParser op_parser;
		const auto end = op_parser.parse_stream(str, op_end);
		if (!IsInvalidStreamPosition(end))
		{
			const bool has_non_empty_address_column = !core::Trim(str.substr(end)).empty();
			if (has_non_empty_address_column)
			{
				return InvalidStreamPosition();
			}
		}
	}

	operation_parser_ = std::move(op_parser_of_label);
	return parse_address_str_with_comment(str, op_end);
}

std::size_t LineParser::try_parse_line_with_label(const std::string_view& str, std::size_t offset)
{
	LabelParser label_parser;
	const auto label_end = label_parser.parse_stream(str, offset);
	if (IsInvalidStreamPosition(label_end))
	{
		return InvalidStreamPosition();
	}
	
	label_parser_ = std::move(label_parser);
	return try_parse_line_without_label(str, label_end, false/*do not go to label parsing*/);
}

std::size_t LineParser::parse_address_str_with_comment(const std::string_view& str, std::size_t offset)
{
	assert(operation_parser_);

	OperationAddressParser parser{operation_parser_->operation().id()};
	const auto end = parser.parse_stream(str, offset);
	if (IsInvalidStreamPosition(end))
	{
		return InvalidStreamPosition();
	}
	address_ = std::move(parser);

	auto comment = core::LeftTrim(str.substr(end));
	if (!comment.empty())
	{
		comment_ = std::move(comment);
		return str.size();
	}

	return end;
}

ConstOptionalRef<std::string_view> LineParser::comment() const
{
	return comment_;
}

ConstOptionalRef<LabelParser> LineParser::label_parser() const
{
	return label_parser_;
}

ConstOptionalRef<OperationParser> LineParser::operation_parser() const
{
	return operation_parser_;
}

ConstOptionalRef<OperationAddressParser> LineParser::address() const
{
	return address_;
}

void LineParser::do_clear()
{
	comment_ = std::nullopt;
	label_parser_ = std::nullopt;
	operation_parser_ = std::nullopt;
	address_ = std::nullopt;
}

bool LineParser::has_only_comment() const
{
	return comment_ && IsCommentBeginning(*comment_);
}

bool LineParser::has_inline_comment() const
{
	return comment_ && !IsCommentBeginning(*comment_);
}

