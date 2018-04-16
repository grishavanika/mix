#include <mixal_format/stream_formatter.h>

#include <mixal_parse/line_parser.h>
#include <mixal_parse/parsers_utils.h>

#include <core/string.h>

#include <sstream>
#include <iomanip>
#include <iostream>
#include <algorithm>
#include <iterator>

#include <cassert>
#include <cctype>

namespace mixal_format {

namespace {

using namespace mixal_parse;

std::string ToUpper(const std::string& str)
{
	std::string all_upper_case;
	all_upper_case.reserve(str.size());
	std::transform(str.cbegin(), str.cend(),
		std::back_inserter(all_upper_case),
		[](char ch)
	{
		return static_cast<char>(std::toupper(ch));
	});
	return all_upper_case;
}

const std::size_t k_address_str_width = 20;
const std::size_t k_inline_comment_offset = 5;
const std::size_t k_op_offset = 3;

std::string_view LabelString(const LineParser& line_parser)
{
	if (line_parser.label_parser())
	{
		return line_parser.label_parser()->label().name();
	}
	return {};
}

std::string BuildLineWithDefaultFormatring(const LineParser& line_parser)
{
	std::ostringstream stream;

	if (line_parser.has_only_comment())
	{
		stream << core::RightTrim(*line_parser.comment());
		return stream.str();
	}

	stream << std::setw(k_max_symbol_length) << std::left
		<< LabelString(line_parser);

	stream << ' ';

	assert(line_parser.operation_parser());
	const auto op_id = line_parser.operation_parser()->operation().id();

	stream << std::setw(k_max_operation_str_length) << std::left
		<< OperationIdToString(op_id);

	stream << std::string(k_op_offset, ' ');

	// #TODO: remove all spaces if specified

	assert(line_parser.address());

	if (op_id != OperationId::ALF)
	{
		stream << std::setw(k_address_str_width) << std::left
			<< line_parser.address()->str();
	}
	else
	{
		stream << '"' << line_parser.address()->mixal()->alf_text->data() << '"';
	}

	if (line_parser.has_inline_comment())
	{
		stream << std::string(k_inline_comment_offset, ' ')
			<< core::RightTrim(*line_parser.comment());
	}

	return stream.str();
}

std::string BuildLineWithMDKFormatting(const LineParser& line_parser)
{
	std::ostringstream stream;

	if (line_parser.has_only_comment())
	{
		stream << core::RightTrim(*line_parser.comment());
		return stream.str();
	}

	stream << LabelString(line_parser);
	stream << '\t';

	assert(line_parser.operation_parser());
	const auto op_id = line_parser.operation_parser()->operation().id();
	stream << OperationIdToString(op_id);
	stream << '\t';

	assert(line_parser.address());
	if (op_id != OperationId::ALF)
	{
		stream << line_parser.address()->str();
	}
	else
	{
		stream << '"' << line_parser.address()->mixal()->alf_text->data() << '"';
	}

	if (line_parser.has_inline_comment())
	{
		stream << '\t' << core::RightTrim(*line_parser.comment());
	}

	return stream.str();
}

std::string BuildLine(const LineParser& line_parser, const FormatOptions& options)
{
	if (options.mdk_compatible)
	{
		return BuildLineWithMDKFormatting(line_parser);
	}
	else
	{
		return BuildLineWithDefaultFormatring(line_parser);
	}
}

std::string FormatLine(const std::string& line, const FormatOptions& options /*= {}*/)
{
	std::string_view line_view{core::Trim(line)};
	if (line_view.empty())
	{
		return {};
	}

	LineParser line_parser;
	if (IsInvalidStreamPosition(line_parser.parse_stream(line_view)))
	{
		std::cerr << "Failed to parse MIXAL line of code: \n\t"
			<< line_view << "\n";
		return line;
	}

	return BuildLine(line_parser, options);
}

} // namespace

void FormatStream(std::istream& in, std::ostream& out, const FormatOptions& options /*= {}*/)
{
	if (!options.title_comment.empty())
	{
		out << "* " << options.title_comment << "\n";
	}

	std::string line;
	while (getline(in, line))
	{
		line = options.make_all_uppercase ? ToUpper(line) : line;
		
		const auto formatted_line = FormatLine(line, options);
		if (formatted_line.empty() && options.mdk_compatible)
		{
			continue;
		}

		out << formatted_line << '\n';
		assert(out);
	}
}

} // namespace mixal_format
