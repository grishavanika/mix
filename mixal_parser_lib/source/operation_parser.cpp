#include <mixal/operation_parser.h>
#include <mixal/parse_exceptions.h>

using namespace mixal;

bool OperationParser::try_parse(std::string_view str)
{
	clear();

	const auto id = OperationIdFromString(str);
	if (id == OperationId::Unknown)
	{
		return false;
	}

	id_ = id;
	str_ = str;
	return true;
}

void OperationParser::parse(std::string_view str)
{
	if (!try_parse(str))
	{
		throw UnknownOperationId{};
	}
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

void OperationParser::clear()
{
	id_ = OperationId::Unknown;
	str_ = {};
}
