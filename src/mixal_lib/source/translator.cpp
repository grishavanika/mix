#include <mixal/translator.h>
#include <mixal/exceptions.h>

#include <mix/char_table.h>

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
		throw InvalidALFText{text};
	}

	Word::BytesArray bytes;
	std::transform(data.cbegin(), data.cend(), bytes.begin(),
		[&](char ch)
	{
		bool converted = false;
		const auto char_byte = mix::CharToByte(ch, &converted);
		if (!converted)
		{
			throw InvalidALFText{text};
		}
		return char_byte;
	});

	return Word{std::move(bytes)};
}

Word Translator::evaluate(const WValue& /*wvalue*/) const
{
	return Word{};
}

Word Translator::evaluate(const Expression& /*expr*/) const
{
	return Word{};
}

Word Translator::evaluate(const Number& n) const
{
	const auto v = n.value();
	if (v > Word::k_max_abs_value)
	{
		throw TooBigWordValueError{n};
	}

	return Word{static_cast<int>(v)};
}

Word Translator::evaluate(const Symbol& symbol) const
{
	assert(!symbol.is_local());

	return defined_symbol(symbol);
}

FutureWord Translator::translate_MIX(
	Operation /*command*/,
	const APart& /*A*/, const IPart& /*I*/, const FPart& /*F*/,
	const Label& /*label*/ /*= {}*/)
{
	return {};
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
		throw SymbolAlreadyDefinedError{symbol, value};
	}
}

const Word& Translator::defined_symbol(const Symbol& symbol) const
{
	// #TODO: `Kind::Here` CAN NOT be queried
	assert(!symbol.is_local());

	auto it = defined_symbols_.find(symbol);
	if (it == defined_symbols_.end())
	{
		throw SymbolIsNotDefinedError{symbol};
	}

	return it->second;
}


