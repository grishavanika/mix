#include <mixal/line_translator.h>
#include <mixal/translator.h>
#include <mixal_parse/line_parser.h>

using namespace mixal_parse;

namespace mixal {

namespace {

// Using this kind of conversion to unify interface of native Translator class
// (that returns `FutureTranslatedWordRef` or simply `TranslatedWord` depending
// on called `translate*()` function)
FutureTranslatedWordRef MakeFutureWord(const TranslatedWord& word)
{
	auto ref = std::make_shared<FutureTranslatedWord>(word.original_address);
	ref->value = word.value;
	return ref;
}

FutureTranslatedWordRef MakeNullFutureWord()
{
	return {};
}

Text QueryALFText(const OperationAddressParser& parser)
{
	assert(parser.is_mixal_operation());
	auto alf_text = parser.mixal()->alf_text;
	auto text = alf_text.value_or(Text{});
	return text;
}

WValue QueryWValue(const OperationAddressParser& parser)
{
	assert(parser.is_mixal_operation());
	auto wvalue_parser = parser.mixal()->w_value_parser;
	auto wvalue = wvalue_parser.value_or(WValueParser{}).value();
	return wvalue;
};

MIXOpParser QueryMIXParsers(const OperationAddressParser& parser)
{
	assert(parser.is_mix_operation());
	return *parser.mix();
}

Operation QueryOperation(const LineParser& line)
{
	assert(line.operation_parser());
	return line.operation_parser()->operation();
}

Label QueryLabel(const LineParser& line)
{
	return line.label_parser().value_or(LabelParser{}).label();
}

OperationAddressParser QueryAddress(const LineParser& line)
{
	const auto operation = QueryOperation(line);
	return line.address().value_or(OperationAddressParser{operation.id()});
}

} // namespace

TranslatedLine::TranslatedLine(FutureTranslatedWordRef&& ref)
	: word_ref{std::move(ref)}
	, end_code{}
{
}

TranslatedLine::TranslatedLine(EndCommandGeneratedCode&& end_code)
	: word_ref{}
	, end_code{std::move(end_code)}
{
}

TranslatedLine TranslateLine(
	Translator& translator,
	const LineParser& line)
{
	if (line.has_only_comment())
	{
		return MakeNullFutureWord();
	}

	const auto operation = QueryOperation(line);
	const auto label = QueryLabel(line);
	const auto address = QueryAddress(line);

	switch (operation.id())
	{
	case OperationId::EQU:
		translator.translate_EQU(
			QueryWValue(address), label);
		return MakeNullFutureWord();
	case OperationId::ORIG:
		translator.translate_ORIG(
			QueryWValue(address), label);
		return MakeNullFutureWord();
	case OperationId::CON:
		return MakeFutureWord(translator.translate_CON(
			QueryWValue(address), label));
	case OperationId::ALF:
		return MakeFutureWord(translator.translate_ALF(
			QueryALFText(address), label));
	case OperationId::END:
		return translator.translate_END(
			QueryWValue(address), label);
	default:
	{
		const auto mix = QueryMIXParsers(address);
		return translator.translate_MIX(
			operation,
			mix.address_parser.address(),
			mix.index_parser.index(),
			mix.field_parser.field(),
			label);
	}
	};
}

} // namespace mixal
