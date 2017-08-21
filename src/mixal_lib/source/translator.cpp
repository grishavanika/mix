#include <mixal/translator.h>
#include <mixal/exceptions.h>
#include <mixal/operations_calculator.h>

#include <mix/char_table.h>
#include <mix/word_field.h>

using namespace mixal;

Translator::Translator(const DefinedSymbols& symbols /*= {}*/, int current_address /*= 0*/)
	: current_address_{current_address}
	, defined_symbols_{symbols}
{
}

Word Translator::evaluate(const Text& text) const
{
	const auto& data = text.data();
	if (data.size() != Word::k_bytes_count)
	{
		throw InvalidALFText{};
	}

	Word::BytesArray bytes;
	std::transform(data.cbegin(), data.cend(), bytes.begin(),
		[&](char ch)
	{
		return process_ALF_text_char(ch);
	});

	return bytes;
}

Byte Translator::process_ALF_text_char(char ch) const
{
	bool converted = false;
	const auto char_byte = mix::CharToByte(ch, &converted);
	if (!converted)
	{
		throw InvalidALFText{};
	}
	return char_byte;
}

Word Translator::evaluate(const BasicExpression& expr) const
{
	if (expr.is_current_address())
	{
		return current_address_;
	}
	else if (expr.is_number())
	{
		return evaluate(expr.as_number());
	}
	else if (expr.is_symbol())
	{
		return evaluate(expr.as_symbol());
	}

	// #TODO: add UNREACHABLE() or similar macro
	assert(false);
	return {};
}

Word Translator::evaluate(const WValue& wvalue) const
{
	assert(!wvalue.tokens.empty());

	Word value;
	for (const auto& token : wvalue.tokens)
	{
		process_wvalue_token(token, value);
	}

	return value;
}

void Translator::process_wvalue_token(const WValueToken& token, Word& dest) const
{
	const auto part = evaluate(token.expression);
	const auto field = evaluate_wvalue_field(token.field);

	// Note: part of `CommandProcessor::do_store()` implementation
	const bool take_value_sign = field.includes_sign();
	dest.set_value(
		part.value(field.shift_bytes_right(), take_value_sign),
		field,
		false/*do not overwrite sign*/);
}

WordField Translator::evaluate_wvalue_field(const std::optional<Expression>& field_expr) const
{
	WordField field{0, Word::k_bytes_count};
	if (field_expr)
	{
		const auto value = evaluate(*field_expr).value();
		field = WordField::FromByte(int{value});
	}

	if ((field.left() > Word::k_bytes_count) ||
		(field.right() > Word::k_bytes_count))
	{
		throw InvalidWValueField{};
	}
	return field;
}

Word Translator::evaluate(const Expression& expr) const
{
	assert(expr.is_valid());
	const auto& tokens = expr.tokens();

	auto left_token = tokens[0];
	auto value = CalculateOptionalUnaryOperation(
		left_token.unary_op, evaluate(left_token.basic_expr));

	for (std::size_t i = 1, count = tokens().size(); i < count; ++i)
	{
		auto right_token = tokens[i];
		value = CalculateBinaryOperation(
			*left_token.binary_op, std::move(value), evaluate(right_token.basic_expr));

		left_token = right_token;
	}

	return value;
}

Word Translator::evaluate(const Number& n) const
{
	const auto v = n.value();
	if (v > Word::k_max_abs_value)
	{
		throw TooBigWordValueError{n};
	}

	return static_cast<int>(v);
}

Word Translator::evaluate(const Symbol& symbol) const
{
	assert(!symbol.is_local());

	return defined_symbol(symbol);
}

void Translator::set_current_address(int address)
{
	assert(address >= 0);
	current_address_ = address;
}

int Translator::current_address() const
{
	return current_address_;
}

void Translator::define_symbol(const Symbol& symbol, const Word& value)
{
	// #TODO: only `Kind::Here` Symbols can be defined
	assert(!symbol.is_local());

	const auto it = defined_symbols_.insert(std::make_pair(symbol, value));
	const bool inserted = it.second;
	if (!inserted)
	{
		throw DuplicateSymbolDefinitionError{symbol, value};
	}
}

const Word& Translator::defined_symbol(const Symbol& symbol) const
{
	// #TODO: `Kind::Here` CAN NOT be queried
	assert(!symbol.is_local());

	auto it = defined_symbols_.find(symbol);
	if (it == defined_symbols_.end())
	{
		throw UndefinedSymbolError{symbol};
	}

	return it->second;
}


void Translator::translate_EQU(const WValue& /*value*/, const Label& /*label*/ /*= {}*/)
{
}

void Translator::translate_ORIG(const WValue& /*value*/, const Label& /*label*/ /*= {}*/)
{

}

void Translator::translate_CON(const WValue& /*value*/, const Label& /*label*/ /*= {}*/)
{

}

void Translator::translate_ALF(const Text& /*text*/, const Label& /*label*/ /*= {}*/)
{

}

void Translator::translate_END(const WValue& /*value*/, const Label& /*label*/ /*= {}*/)
{

}

FutureWord Translator::translate_MIX(
	Operation /*command*/,
	const APart& /*A*/, const IPart& /*I*/, const FPart& /*F*/,
	const Label& /*label*/ /*= {}*/)
{
	return {};
}


