#pragma once
#include <mix/word.h>
#include <mix/word_value.h>
#include <mix/word_field.h>

#include <mixal_parse/types/all.h>

namespace mixal {

using Word				= mix::Word;
using Byte				= mix::Byte;
using WordValue			= mix::WordValue;
using WordField			= mix::WordField;

using Expression		= mixal_parse::Expression;
using UnaryOperation	= mixal_parse::UnaryOperation;
using BinaryOperation	= mixal_parse::BinaryOperation;
using WValue			= mixal_parse::WValue;
using OperationId		= mixal_parse::OperationId;
using Text				= mixal_parse::Text;
using Label				= mixal_parse::Label;
using Symbol			= mixal_parse::Symbol;
using Number			= mixal_parse::Number;
using Operation			= mixal_parse::Operation;
using BasicExpression	= mixal_parse::BasicExpression;

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
