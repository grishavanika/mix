#include <mixal/operation_parser.h>
#include <mixal/parse_exceptions.h>

using namespace mixal;

bool OperationParser::try_parse(std::string_view str)
{
	do_clear();

	const auto id = OperationIdFromString(str);
	if (id == OperationId::Unknown)
	{
		return false;
	}

	id_ = id;
	return true;
}

void OperationParser::do_parse(std::string_view str)
{
	if (!try_parse(str))
	{
		throw UnknownOperationId{};
	}
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

void OperationParser::do_clear()
{
	id_ = OperationId::Unknown;
}
