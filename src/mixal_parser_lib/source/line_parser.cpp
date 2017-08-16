#include <mixal/line_parser.h>

#include <core/string.h>

#include <algorithm>

#include <cassert>
#include <cctype>

using namespace mixal;

namespace {

const char k_comment_begin_char = '*';
const char k_whitespaces[] = " \t";

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

std::size_t LineParser::try_parse_line_without_label(const std::string_view& str, std::size_t offset)
{
	OperationParser op_parser;
	const auto op_end = op_parser.parse_stream(str, offset);
	if (IsInvalidStreamPosition(op_end))
	{
		return InvalidStreamPosition();
	}

	operation_ = std::move(op_parser);
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
	
	label_ = std::move(label_parser);
	return try_parse_line_without_label(str, label_end);
}

std::size_t LineParser::parse_address_str_with_comment(const std::string_view& str, std::size_t offset)
{
	auto line = core::LeftTrim(str.substr(offset));

	const auto comment_start = core::FindIf(line, &islower);

	address_str_ = core::RightTrim(line.substr(0, comment_start));
	
	if (!IsInvalidStreamPosition(comment_start))
	{
		comment_ = line.substr(comment_start);
	}

	return str.size();
}

ConstOptionalRef<std::string_view> LineParser::comment() const
{
	return comment_;
}

ConstOptionalRef<LabelParser> LineParser::label() const
{
	return label_;
}

ConstOptionalRef<OperationParser> LineParser::operation() const
{
	return operation_;
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

