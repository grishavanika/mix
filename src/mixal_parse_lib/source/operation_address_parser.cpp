#include <mixal_parse/operation_address_parser.h>

#include <cassert>
#include <cctype>

using namespace mixal_parse;

namespace {

const std::size_t k_ALF_op_length = 5;

bool ALFStartsWithOneSpace(const std::string_view& str, std::size_t offset)
{
	return (str.size() > (offset + 2)) &&
		std::isspace(str[offset]) &&
		!std::isspace(str[offset + 1]);
}

bool ALFStartsWithTwoSpaces(const std::string_view& str, std::size_t offset)
{
	return (str.size() > (offset + 3)) &&
		std::isspace(str[offset]) &&
		std::isspace(str[offset + 1]);
}

bool IsValidALFText(const std::string_view& /*alf_text*/)
{
	// #TODO: implement validation
	return true;
}

} // namespace

/*explicit*/ OperationAddressParser::OperationAddressParser(OperationId operation)
	: op_{operation}
{
	assert(IsMIXOperation(op_) || IsMIXALOperation(op_));
}

std::size_t OperationAddressParser::do_parse_stream(std::string_view str, std::size_t offset)
{
	if (IsMIXOperation(op_))
	{
		return parse_as_mix(str, offset);
	}
	else if (IsMIXALOperation(op_))
	{
		return parse_as_mixal(str, offset);
	}

	return InvalidStreamPosition();
}

std::size_t OperationAddressParser::parse_as_mix(const std::string_view& str, std::size_t offset)
{
	auto try_parse = [&](ParserBase& p)
	{
		if (!IsInvalidStreamPosition(offset))
		{
			offset = p.parse_stream(str, offset);
		}
	};

	MIXOpParser mix;

	try_parse(mix.address_parser);
	try_parse(mix.index_parser);
	try_parse(mix.field_parser);

	if (!IsInvalidStreamPosition(offset))
	{
		mix_ = std::move(mix);
	}
	return offset;
}

std::size_t OperationAddressParser::parse_as_mixal(const std::string_view& str, std::size_t offset)
{
	if (op_ == OperationId::ALF)
	{
		return parse_mixal_alf_op(str, offset);
	}

	return parse_mixal_op_as_wvalue(str, offset);
}

std::size_t OperationAddressParser::parse_mixal_op_as_wvalue(const std::string_view& str, std::size_t offset)
{
	WValueParser parser;
	const auto end = parser.parse_stream(str, offset);
	if (!IsInvalidStreamPosition(end))
	{
		MIXALOpParser mixal;
		mixal.w_value_parser = std::move(parser);
		mixal_ = std::move(mixal);
	}
	return end;
}

std::size_t OperationAddressParser::parse_mixal_alf_op(const std::string_view& str, std::size_t offset)
{
	const auto quotes_end = try_parse_alf_with_quotes(str, offset);
	if (!IsInvalidStreamPosition(quotes_end))
	{
		return quotes_end;
	}

	return try_parse_standard_alf(str, offset);
}

std::size_t OperationAddressParser::try_parse_standard_alf(const std::string_view& str, std::size_t offset)
{
	if (ALFStartsWithOneSpace(str, offset))
	{
		offset += 1;
	}
	else if (ALFStartsWithTwoSpaces(str, offset))
	{
		offset += 2;
	}
	else
	{
		return InvalidStreamPosition();
	}

	// #TODO: do not expect string length to be 5.
	// Simply read 5 chars or to the end of line if the length is less
	if (str.size() < (offset + k_ALF_op_length))
	{
		return InvalidStreamPosition();
	}

	return set_alf_text(str, offset);
}

std::size_t OperationAddressParser::try_parse_alf_with_quotes(const std::string_view& str, std::size_t offset)
{
	auto start = ExpectFirstNonWhiteSpaceChar('"', str, offset);
	if (IsInvalidStreamPosition(start))
	{
		return InvalidStreamPosition();
	}
	// Skip `"`
	++start;

	const bool has_anough_length = (str.size() > (start + k_ALF_op_length));
	if (!has_anough_length)
	{
		return InvalidStreamPosition();
	}
	const bool closes_quotes = (str[start + k_ALF_op_length] == '"');
	if (!closes_quotes)
	{
		return InvalidStreamPosition();
	}

	const auto end = set_alf_text(str, start);
	if (IsInvalidStreamPosition(end))
	{
		return InvalidStreamPosition();
	}

	return (end + 1);
}

std::size_t OperationAddressParser::set_alf_text(const std::string_view& str, std::size_t offset)
{
	MIXALOpParser mixal;
	mixal.alf_text = str.substr(offset, k_ALF_op_length);
	if (!IsValidALFText(*mixal.alf_text))
	{
		return InvalidStreamPosition();
	}

	mixal_ = std::move(mixal);
	return (offset + k_ALF_op_length);
}

void OperationAddressParser::do_clear()
{
	mix_ = std::nullopt;
	mixal_ = std::nullopt;
}

ConstOptionalRef<MIXOpParser> OperationAddressParser::mix() const
{
	return mix_;
}

ConstOptionalRef<MIXALOpParser> OperationAddressParser::mixal() const
{
	return mixal_;
}

bool OperationAddressParser::is_mix_operation() const
{
	return static_cast<bool>(mix_);
}

bool OperationAddressParser::is_mixal_operation() const
{
	return static_cast<bool>(mixal_);
}
