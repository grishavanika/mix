#pragma once
#include <mixal/config.h>
#include <mixal/translator.h>

#include <mixal_parse/line_parser.h>

#include <list>


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

struct TranslatedProgram
{
	std::vector<TranslatedWord> commands;
	int start_address{0};
	bool completed{false};
};

MIXAL_LIB_EXPORT
TranslatedLine TranslateLine(
	Translator& translator,
	const mixal_parse::LineParser& line);

class MIXAL_LIB_EXPORT ProgramTranslator
{
public:
	enum class Status
	{
		PartiallyTranslated,
		Completed
	};

	ProgramTranslator(Translator& translator);

	// After END command translating, `program().completed` is set to `true`
	// and this function returns `Status::Completed`
	// 
	// Note: empty lines (or lines with only whitespaces) are ignored
	Status translate_line(const std::string& line);

	const TranslatedProgram& program() const;

private:
	std::string_view prepare_line(const std::string& line);
	std::optional<mixal_parse::LineParser> parse_line(const std::string_view& line) const;
	void update_code(TranslatedLine&& translated);
	void update_code(const Translator::EndCommandGeneratedCode& end_code);
	void finilize_program(int start_address);
	void clear_state();

private:
	Translator& translator_;
	TranslatedProgram program_;
	std::list<std::string> cached_lines_;
	std::vector<FutureTranslatedWordRef> unresolved_words_;
};

} // namespace mixal

