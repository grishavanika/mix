#pragma once
#include <mix/word.h>

#include <mixal_parse/types/all.h>

namespace mixal {

using Word			= mix::Word;

using Expression	= mixal_parse::Expression;
using WValue		= mixal_parse::WValue;
using OperationId	= mixal_parse::OperationId;
using Text			= mixal_parse::Text;
using Label			= mixal_parse::Label;
using Symbol		= mixal_parse::Symbol;
using Number		= mixal_parse::Number;
using Operation		= mixal_parse::Operation;

class FutureWord
{
	int address;
};

class APart
{
	bool is_defaulted() const;
};

class IPart
{
	bool is_defaulted() const;
};

class FPart
{
	bool is_defaulted() const;
};

} // namespace mixal
