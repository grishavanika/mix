#include <mixal/operation_info.h>

namespace mixal {
namespace {

// #TODO: map all OperationId to command id and default field

} // namespace

OperationInfo QueryOperationInfo(const Operation& op)
{
	return {op.id(), 0, Word::MaxField(), 0};
}

} // namespace mixal
