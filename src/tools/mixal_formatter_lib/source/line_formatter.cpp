#include <mixal_formatter/line_formatter.h>
#include <mixal/line_parser.h>
#include <mixal/parsers_utils.h>

#include <core/string.h>

#include <sstream>
#include <iomanip>

#include <cassert>

using namespace mixal;

namespace mixal_formatter {

namespace {

const std::size_t k_address_str_width = 30;
const std::size_t k_inline_comment_offset = 5;
const std::size_t k_op_offset = 3;

std::string BuildLine(const LineParser& line_parser, const FormatOptions& options)
{
	(void)options;

	std::ostringstream stream;

	if (line_parser.has_only_comment())
	{
		stream << core::RightTrim(*line_parser.comment());
		return stream.str();
	}

	stream << std::setw(k_max_symbol_length) << std::left
		<< (line_parser.label() ? line_parser.label()->name() : "");

	stream << ' ';

	assert(line_parser.operation());

	stream << std::setw(k_max_operation_str_length) << std::left
		<< OperationIdToString(line_parser.operation()->id());

	stream << std::string(k_op_offset, ' ');

	// #TODO: remove all spaces if specified

	// #TODO: special handling of `ALF` command

	stream << std::setw(k_address_str_width) << std::left
		<< line_parser.address_str();

	if (line_parser.has_inline_comment())
	{
		stream << std::string(k_inline_comment_offset, ' ')
			<< core::RightTrim(*line_parser.comment());
	}

	return stream.str();
}

} // namespace

std::string FormatLine(const std::string& line, const FormatOptions& options /*= {}*/)
{
	std::string_view line_view{line};
	if (core::Trim(line_view).empty())
	{
		return {};
	}

	LineParser line_parser;
	if (IsInvalidStreamPosition(line_parser.parse_stream(line_view)))
	{
		assert(!"Failed to parse MIXAL line");
		return line;
	}

	return BuildLine(line_parser, options);
}

} // namespace mixal_formatter

