#pragma once
#include <mixal_parse/config.h>
#include <mixal_parse/operation_id.h>

namespace mixal_parse {

class MIXAL_PARSE_LIB_EXPORT Operation
{
public:
	Operation();
	Operation(OperationId id);

	OperationId id() const;

	bool is_mix() const;
	bool is_mixal() const;

	bool is_valid() const;

private:
	OperationId id_;
};

} // namespace mixal_parse

////////////////////////////////////////////////////////////////////////////////

namespace mixal_parse {

inline Operation::Operation()
	: Operation{OperationId::Unknown}
{
}

inline Operation::Operation(OperationId id)
	: id_{id}
{
}

inline bool Operation::is_mix() const
{
	return IsMIXOperation(id_);
}

inline bool Operation::is_mixal() const
{
	return IsMIXALOperation(id_);
}

inline bool Operation::is_valid() const
{
	return (id_ > OperationId::Unknown) &&
		(id_ < OperationId::Count);
}

inline OperationId Operation::id() const
{
	return id_;
}

} // namespace mixal_parse

