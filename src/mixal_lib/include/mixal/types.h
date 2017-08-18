#pragma once
#include <mix/word.h>

#include <mixal_parse/expression.h>
#include <mixal_parse/operation_id.h>

namespace mixal {

using Word = mix::Word;

using Expression = mixal_parse::Expression;
using WValue = mixal_parse::WValue;
using OperationId = mixal_parse::OperationId;

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

class Symbol
{

};

class ALFText
{

};

class Number
{

};

class Label
{

};

} // namespace mixal
