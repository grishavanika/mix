#pragma once
#include <mixal/types.h>

namespace mix
{
    class Command;
    class Word;
} // namespace mix

namespace mixal
{
    struct OperationInfo
    {
	    OperationId id = OperationId::Unknown;
	    int computer_command = -1;
	    WordField default_field = WordField(0, 0);
	    int default_index = -1;
    };

    OperationInfo QueryOperationInfo(const Operation& op);
    OperationInfo QueryOperationInfo(const mix::Word& w);
    OperationInfo QueryOperationInfo(const mix::Command& cmd);
} // namespace mixal

