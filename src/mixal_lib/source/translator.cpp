#include <mixal/translator.h>
#include <mixal/exceptions.h>
#include <mixal/operations_calculator.h>

#include <mix/char_table.h>
#include <mix/word_field.h>

#include <functional>
#include <algorithm>

using namespace mixal;

Translator::Translator(
	const DefinedSymbols& symbols /*= {}*/,
	const DefinedLocalSymbols& local_symbols /*= {}*/,
	int current_address /*= 0*/)
		: current_address_{current_address}
		, defined_symbols_{symbols}
		, defined_local_symbols_{local_symbols}
{
	prepare_local_addresses(defined_local_symbols_);
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
	assert(wvalue.is_valid());

	Word value;
	for (const auto& token : wvalue.tokens())
	{
		process_wvalue_token(token, value);
	}

	return value;
}

void Translator::process_wvalue_token(const WValue::Token& token, Word& dest) const
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

	for (std::size_t i = 1, count = tokens.size(); i < count; ++i)
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
	return query_defined_symbol(symbol);
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
	if (symbol.is_local())
	{
		define_local_symbol(symbol, value.value());
	}
	else
	{
		define_usual_symbol(symbol, value);
	}
}

void Translator::define_usual_symbol(const Symbol& symbol, const Word& value)
{
	const auto it = defined_symbols_.insert(std::make_pair(symbol, value));
	const bool inserted = it.second;
	if (!inserted)
	{
		throw DuplicateSymbolDefinitionError{symbol, value};
	}
}

void Translator::define_local_symbol(const Symbol& symbol, int address)
{
	if (symbol.kind() != LocalSymbolKind::Here)
	{
		throw InvalidLocalSymbolDefinition{symbol};
	}
	const auto id = symbol.local_id();
	auto& addresses = defined_local_symbols_[id];
	addresses.push_back(address);
	prepare_local_addresses(addresses);
}

void Translator::prepare_local_addresses(Addresses& addresses) const
{
	sort(addresses.begin(), addresses.end(), std::greater<>{});
}

void Translator::prepare_local_addresses(DefinedLocalSymbols& local_symbols) const
{
	for (auto& addresses : local_symbols)
	{
		prepare_local_addresses(addresses.second);
	}
}

Word Translator::query_defined_symbol(const Symbol& symbol, int near_address /*= -1*/) const
{
	if (symbol.is_local())
	{
		return query_local_symbol(symbol, near_address);
	}
	else
	{
		return query_usual_symbol(symbol);
	}
}

Word Translator::query_usual_symbol(const Symbol& symbol) const
{
	auto it = defined_symbols_.find(symbol);
	if (it == defined_symbols_.end())
	{
		throw UndefinedSymbolError{symbol};
	}

	return it->second;
}

Word Translator::query_local_symbol(const Symbol& symbol, int near_address) const
{
	if (symbol.kind() != LocalSymbolKind::Backward)
	{
		throw InvalidLocalSymbolReference{symbol};
	}

	auto value = find_local_symbol(symbol, near_address);
	if (!value)
	{
		throw InvalidLocalSymbolReference{symbol};
	}

	return *value;
}

const int* Translator::find_local_symbol(const Symbol& symbol, int near_address) const
{
	const auto addresses_it = defined_local_symbols_.find(symbol.local_id());
	if (addresses_it == defined_local_symbols_.end())
	{
		return nullptr;
	}

	const auto& addresses = addresses_it->second;
	auto address_it = lower_bound(
		addresses.cbegin(), addresses.cend(),
		near_address, std::greater<>{});

	return (address_it != addresses.cend()) ? &*address_it : nullptr;
}

bool Translator::is_defined_symbol(const Symbol& symbol, int near_address /*= -1*/) const
{
	if (symbol.is_local())
	{
		return is_defined_local_symbol(symbol, near_address);
	}
	else
	{
		return is_defined_usual_symbol(symbol);
	}
}

bool Translator::is_defined_usual_symbol(const Symbol& symbol) const
{
	return (defined_symbols_.find(symbol) != defined_symbols_.end());
}

bool Translator::is_defined_local_symbol(const Symbol& symbol, int near_address) const
{
	if (symbol.kind() != LocalSymbolKind::Backward)
	{
		return false;
	}

	return (find_local_symbol(symbol, near_address) != nullptr);
}

void Translator::define_label_if_valid(const Label& label, const Word& value)
{
	if (label.empty())
	{
		return;
	}

	define_symbol(label.symbol(), value);
}

void Translator::translate_EQU(const WValue& value, const Label& label /*= {}*/)
{
	define_label_if_valid(label, evaluate(value));
}

void Translator::translate_ORIG(const WValue& value, const Label& label /*= {}*/)
{
	const auto address = evaluate(value);
	define_label_if_valid(label, address);
	set_current_address(address.value());
}

AddressedWord Translator::translate_CON(const WValue& wvalue, const Label& label /*= {}*/)
{
	define_label_if_valid(label, current_address_);

	AddressedWord result{current_address_, evaluate(wvalue)};
	increase_current_address();
	return result;
}

AddressedWord Translator::translate_ALF(const Text& text, const Label& label /*= {}*/)
{
	define_label_if_valid(label, current_address_);

	AddressedWord result{current_address_, evaluate(text)};
	increase_current_address();
	return result;
}

void Translator::translate_END(const WValue& /*value*/, const Label& /*label*/ /*= {}*/)
{

}

FutureWord Translator::translate_MIX(
	Operation /*command*/,
	const Address& /*A*/, const Index& /*I*/, const Field& /*F*/,
	const Label& /*label*/ /*= {}*/)
{
	return {};
}

void Translator::increase_current_address()
{
	++current_address_;
}

