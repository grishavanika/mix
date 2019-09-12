#pragma once
#include <mixal/config.h>
#include <mixal/translator.h>

#include <mixal_parse/line_parser.h>
#include <mixal_parse/types/operation_id.h>

#include <list>


namespace mixal {

struct TranslatedLine
{
	using EndCommandGeneratedCode = Translator::EndCommandGeneratedCode;

	FutureTranslatedWordRef word_ref;
	// Special case for END. Will be valid only
	// if translated line contains END
	std::optional<EndCommandGeneratedCode> end_code;

    OperationId operation_id;

    explicit TranslatedLine();
	TranslatedLine(FutureTranslatedWordRef&& ref);
	TranslatedLine(EndCommandGeneratedCode&& end_code);

    bool is_valid() const
    {
        return (word_ref || end_code);
    }
};

struct TranslatedProgram
{
	std::vector<TranslatedWord> commands;
	int start_address{-1};
};

MIXAL_LIB_EXPORT
TranslatedLine TranslateLine(
	Translator& translator,
	const mixal_parse::LineParser& line);

class MIXAL_LIB_EXPORT LinesTranslator
{
public:
	LinesTranslator(Translator& translator);

	// After END command translating, `program()` is completed
    // (`start_address` is valid, i.e, >= 0).
    // 
	// Note: empty lines (or lines with only whitespaces) are ignored
    TranslatedLine translate(const std::string& line);

private:
	std::string_view prepare_line(const std::string& line);
	std::optional<mixal_parse::LineParser> parse_line(const std::string_view& line) const;

private:
	Translator& translator_;
	std::list<std::string> cached_lines_;
};

} // namespace mixal

