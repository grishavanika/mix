#pragma once
#include <mix/word.h>
#include <mix/word_value.h>
#include <mix/word_field.h>

#include <mixal_parse/types/all.h>

#include <memory>

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
using LocalSymbolKind	= mixal_parse::LocalSymbolKind;
using LocalSymbolId		= mixal_parse::LocalSymbolId;
using Text				= mixal_parse::Text;
using Label				= mixal_parse::Label;
using Symbol			= mixal_parse::Symbol;
using Number			= mixal_parse::Number;
using Operation			= mixal_parse::Operation;
using BasicExpression	= mixal_parse::BasicExpression;
using Address			= mixal_parse::Address;
using Index				= mixal_parse::Index;
using Field				= mixal_parse::Field;

struct TranslatedWord
{
	int original_address{-1};
	Word value{};
};

struct FutureTranslatedWord :
	public TranslatedWord
{
	std::vector<Symbol> forward_references;
	Address unresolved_address;

	FutureTranslatedWord(int address, std::vector<Symbol> references = {})
		: forward_references{std::move(references)}
	{
		original_address = address;
	}

	bool is_ready() const
	{
		return forward_references.empty();
	}

	TranslatedWord translated_word() const
	{
		return *this;
	}
};

// Note: using `std::shared_ptr` for simplicity purpose. It can be optimized
// by using simple wrapper around `TranslatedWord` with reference to `Translator`
using FutureTranslatedWordRef = std::shared_ptr<const FutureTranslatedWord>;
using FutureTranslatedWordShared = std::shared_ptr<FutureTranslatedWord>;

} // namespace mixal
