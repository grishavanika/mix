#pragma once
#include <mixal/types.h>

namespace mixal {

struct OperationInfo
{
	OperationId id;
	int computer_command;
	WordField default_field;
	int default_index;
};

OperationInfo QueryOperationInfo(const Operation& op);

} // namespace mixal

