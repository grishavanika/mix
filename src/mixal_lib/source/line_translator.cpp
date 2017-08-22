#include <mixal/line_translator.h>
#include <mixal/translator.h>
#include <mixal_parse/line_parser.h>

using namespace mixal_parse;

namespace mixal {

namespace {

FutureTranslatedWordRef TranslatedWordToFutureWordRef(const TranslatedWord& word)
{
	auto ref = std::make_shared<FutureTranslatedWord>(word.original_address);
	ref->value = word.value;
	return ref;
}

} // namespace

FutureTranslatedWordRef TranslateLine(
	Translator& translator,
	const LineParser& line)
{
	if (line.has_only_comment())
	{
		return {};
	}

	assert(line.operation_parser());

	const Label label = line.label_parser()
		.value_or(LabelParser{}).label();
	const Operation operation = line.operation_parser()
		.value_or(OperationParser{}).operation();
	const auto address_parser = line.address()
		.value_or(OperationAddressParser{operation.id()});

	auto query_address_w_value = [&]()
	{
		assert(address_parser.is_mixal_operation());
		auto wvalue = address_parser.mixal()->w_value_parser.value_or(WValueParser{}).value();
		return wvalue;
	};

	auto query_address_alf_text = [&]()
	{
		assert(address_parser.is_mixal_operation());
		auto text = address_parser.mixal()->alf_text.value_or(Text{std::string_view{}});
		return text;
	};

	auto query_address_mix = [&]()
	{
		assert(address_parser.is_mix_operation());
		return *address_parser.mix();
	};

	switch (operation.id())
	{
	case OperationId::EQU:
		translator.translate_EQU(query_address_w_value(), label);
		return {};
	case OperationId::ORIG:
		translator.translate_ORIG(query_address_w_value(), label);
		return {};
	case OperationId::CON:
		return TranslatedWordToFutureWordRef(
			translator.translate_CON(query_address_w_value(), label));
	case OperationId::ALF:
		return TranslatedWordToFutureWordRef(
			translator.translate_ALF(query_address_alf_text(), label));
	case OperationId::END:
		translator.translate_END(query_address_w_value(), label);
		return {};
	default:
	{
		const auto mix = query_address_mix();
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
