#pragma once
#include <mixal/translator.h>

#include <core/optional.h>

namespace mixal_parse {

class LineParser;

} // namespace mixal_parse

namespace mixal {

struct TranslatedLine
{
	using EndCommandGeneratedCode = Translator::EndCommandGeneratedCode;

	FutureTranslatedWordRef word_ref;
	// Special case for END. Will be valid only
	// if translated line contains END
	std::optional<EndCommandGeneratedCode> end_code;

	TranslatedLine(FutureTranslatedWordRef&& ref);
	TranslatedLine(EndCommandGeneratedCode&& end_code);
};

TranslatedLine TranslateLine(
	Translator& translator,
	const mixal_parse::LineParser& line);

} // namespace mixal

