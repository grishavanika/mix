#include <mixal/operation_parser.h>
#include <mixal/parse_exceptions.h>

using namespace mixal;

void OperationParser::parse(std::string_view str)
{
	const auto id = OperationIdFromString(str);
	if (id == OperationId::Unknown)
	{
		throw UnknownOperationId{};
	}

	id_ = id;
	str_ = str;
}

std::string_view OperationParser::str() const
{
	return str_;
}

bool OperationParser::is_pseudo_operation() const
{
	return (id_ >= OperationId::PseudoBegin) &&
		(id_ <= OperationId::PseudoEnd);
}

bool OperationParser::is_native_operation() const
{
	return (id_ >= OperationId::NativeBegin) &&
		(id_ <= OperationId::NativeEnd);
}

OperationId OperationParser::id() const
{
	return id_;
}

