#include <mixal/translator.h>

using namespace mixal;

Translator::Translator(int current_address /*= 0*/)
	: current_address_{current_address}
{
}

Word Translator::evaluate(const Text& /*text*/) const
{
	return Word{};
}

Word Translator::evaluate(const WValue& /*wvalue*/) const
{
	return Word{};
}

Word Translator::evaluate(const Expression& /*expr*/) const
{
	return Word{};
}

Word Translator::evaluate(const Number& /*n*/) const
{
	return Word{};
}

Word Translator::evaluate(const Symbol& /*symbol*/) const
{
	return Word{};
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
