#include <mixal_parse/operation_parser.h>
#include <mixal_parse/parsers_utils.h>

#include <algorithm>
#include <cctype>

using namespace mixal_parse;

namespace {

std::size_t FirstWhiteSpace(const std::string_view& str, std::size_t offset)
{
	auto first_space_it = std::find_if(str.begin() + offset, str.end(), &isspace);
	if (first_space_it == str.end())
	{
		return str.size();
	}

	return static_cast<std::size_t>(std::distance(str.begin(), first_space_it));
}

} // namespace

std::size_t OperationParser::do_parse_stream(std::string_view str, std::size_t offset)
{
	const auto first_non_space = SkipLeftWhiteSpaces(str, offset);
	if (first_non_space == str.size())
	{
		return InvalidStreamPosition();
	}
	const auto first_space = FirstWhiteSpace(str, first_non_space);

	op_ = OperationIdFromString(str.substr(first_non_space, first_space - first_non_space));
	if (!op_.is_valid())
	{
		return InvalidStreamPosition();
	}

	return first_space;
}

void OperationParser::do_clear()
{
	op_ = {};
}

Operation OperationParser::operation() const
{
	return op_;
}

