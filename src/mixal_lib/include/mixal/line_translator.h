#pragma once
#include <mixal/types.h>

namespace mixal_parse {

class LineParser;

} // namespace mixal_parse

namespace mixal {

class Translator;

FutureTranslatedWordRef TranslateLine(
	Translator& translator,
	const mixal_parse::LineParser& line);

} // namespace mixal

