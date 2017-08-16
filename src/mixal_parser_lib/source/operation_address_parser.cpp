#include <mixal/operation_address_parser.h>

#include <cassert>

using namespace mixal;

/*explicit*/ OperationAddressParser::OperationAddressParser(OperationId operation)
	: op_{operation}
{
	assert(IsMIXOperation(op_) || IsMIXALOperation(op_));
}

std::size_t OperationAddressParser::do_parse_stream(std::string_view /*str*/, std::size_t /*offset*/)
{
	return InvalidStreamPosition();
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
