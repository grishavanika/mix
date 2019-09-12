#include <mixal/line_translator.h>
#include <mixal/translator.h>

using namespace mixal;
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
	// Help Clang: -Wreturn-std-move-in-c++11 
	return std::move(ref);
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
}

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

/*explicit*/ TranslatedLine::TranslatedLine()
    : word_ref()
    , end_code()
    , operation_id(OperationId::Unknown)
{
}

TranslatedLine::TranslatedLine(FutureTranslatedWordRef&& ref)
	: TranslatedLine()
{
    word_ref = std::move(ref);
}

TranslatedLine::TranslatedLine(EndCommandGeneratedCode&& end_code_)
	: TranslatedLine()
{
    end_code = std::move(end_code_);
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
	}
}

} // namespace mixal

LinesTranslator::LinesTranslator(Translator& translator)
	: translator_{translator}
    , cached_lines_{}
{
}

TranslatedLine LinesTranslator::translate(const std::string& line)
{
	auto parser = parse_line(prepare_line(line));
    if (!parser)
    {
        return TranslatedLine();
    }
    auto translated = TranslateLine(translator_, *parser);
    if (const auto op_parser = parser->operation_parser())
    {
        translated.operation_id = op_parser->operation().id();
    }
    return translated;
}

std::string_view LinesTranslator::prepare_line(const std::string& line)
{
	// See `Interpreter::prepare_line()` note
	cached_lines_.push_back(line);
	return cached_lines_.back();
}

std::optional<LineParser> LinesTranslator::parse_line(const std::string_view& line) const
{
	if (core::Trim(line).empty())
	{
		return {};
	}

	LineParser parser;
	const auto pos = parser.parse_stream(line);
	if (IsInvalidStreamPosition(pos))
	{
		// #TODO: introduce ParseError with details
		throw std::runtime_error{"parse error"};
	}
	// Help Clang: -Wreturn-std-move-in-c++11 
	return std::move(parser);
}
