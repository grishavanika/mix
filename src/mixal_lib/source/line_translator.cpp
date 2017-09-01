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

ProgramTranslator::ProgramTranslator(Translator& translator)
	: translator_{translator}
	, program_{}
{
}

ProgramTranslator::Status ProgramTranslator::translate_line(const std::string& line)
{
	if (program_.completed)
	{
		return Status::Completed;
	}

	auto parser = parse_line(prepare_line(line));
	if (parser)
	{
		update_code(TranslateLine(translator_, *parser));
	}

	return program_.completed ? Status::Completed : Status::PartiallyTranslated;
}

const TranslatedProgram& ProgramTranslator::program() const
{
	return program_;
}

std::string_view ProgramTranslator::prepare_line(const std::string& line)
{
	// See `Interpreter::prepare_line()` note
	cached_lines_.push_back(line);
	return cached_lines_.back();
}

std::optional<LineParser> ProgramTranslator::parse_line(const std::string_view& line) const
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
	return parser;
}

void ProgramTranslator::update_code(const Translator::EndCommandGeneratedCode& end_code)
{
	for (auto unresolved_ref : unresolved_words_)
	{
		assert(unresolved_ref->is_ready());
		program_.commands.push_back(unresolved_ref->translated_word());
	}

	for (auto end_symbols : end_code.defined_symbols)
	{
		program_.commands.push_back(end_symbols.second);
	}
}

void ProgramTranslator::update_code(TranslatedLine&& translated)
{
	if (translated.word_ref)
	{
		if (translated.word_ref->is_ready())
		{
			program_.commands.push_back(translated.word_ref->translated_word());
		}
		else
		{
			unresolved_words_.push_back(translated.word_ref);
		}
	}
	else if (translated.end_code)
	{
		update_code(*translated.end_code);
		finilize_program(translated.end_code->start_address);
	}
}

void ProgramTranslator::finilize_program(int start_address)
{
	sort(program_.commands.begin(), program_.commands.end(),
		[](const TranslatedWord& lhs, const TranslatedWord& rhs)
	{
		return (lhs.original_address < rhs.original_address);
	});

	program_.start_address = start_address;
	program_.completed = true;

	clear_state();
}

void ProgramTranslator::clear_state()
{
	cached_lines_.clear();
	unresolved_words_.clear();
	unresolved_words_.shrink_to_fit();
}

