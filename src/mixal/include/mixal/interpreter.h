#pragma once
#include <mixal/options.h>
#include <mixal/optional_stream.h>
#include <mixal/exceptions_handler.h>

#include <mixal/line_translator.h>
#include <mixal/translator.h>
#include <mixal/exceptions.h>
#include <mixal/program_executor.h>

#include <mixal_parse/line_parser.h>

#include <mix/command.h>

#include <core/string.h>

#include <cassert>

#include <list>
#include <string>
#include <iomanip>
#include <fstream>

namespace mixal {

namespace parse = mixal_parse;

// #XXX: simplify, use LineTranslator
class Interpreter
{
public:
	Interpreter(std::ostream& out, bool show_details);

	void translate_line(const std::string& str);

	int printed_commands_count() const;

private:
	std::size_t lines_number() const;

	std::string_view prepare_line(const std::string& line);
	void add_translated_line(OperationId command, TranslatedLine&& line);

	void add_translated_word(OperationId command,
		FutureTranslatedWordRef&& translated);
	void save_delayed_word(OperationId command,
		FutureTranslatedWordRef translated);

	void update_words();

	void print_translated_word_details(OperationId command,
		FutureTranslatedWordRef translated);
	void print_end_command_details(OperationId command,
		Translator::EndCommandGeneratedCode&& end_code);
	void print_code(OperationId command, const TranslatedWord& word,
		const char* details_text = nullptr);
	void print_details(const char* details_text);

	static OperationId LineOperationId(const parse::LineParser& parser);

private:
	using DelayedWords = FlatMap<FutureTranslatedWordRef, OperationId>;

	OptionalOStream out_;
	Translator translator_;
	DelayedWords delayed_words_;

	std::list<std::string> code_lines_;
	int printed_commands_count_;
};

int RunInterpreter(Options options);
void TranslateStream(Interpreter& interpreter, std::istream& in);

} // namespace mixal

////////////////////////////////////////////////////////////////////////////////

namespace mixal {

inline Interpreter::Interpreter(std::ostream& out, bool show_details)
	: out_{out, show_details}
	, translator_{}
	, delayed_words_{}
	, code_lines_{}
	, printed_commands_count_{0}
{
}

inline std::size_t Interpreter::lines_number() const
{
	return code_lines_.size();
}

int Interpreter::printed_commands_count() const
{
	return printed_commands_count_;
}

inline void Interpreter::save_delayed_word(OperationId command,
	FutureTranslatedWordRef translated)
{
	if (translated && !translated->is_ready())
	{
		delayed_words_.push_back(std::make_pair(std::move(translated), command));
	}
}

inline void Interpreter::print_translated_word_details(OperationId command,
	FutureTranslatedWordRef translated)
{
	if (!translated)
	{
		print_details("no code generated\n");
		return;
	}
	else if (translated->is_ready())
	{
		print_code(command, translated->translated_word());
		return;
	}

	print_details("delaying interpretation because of: ");

	for (const auto& ref : translated->forward_references)
	{
		out_ << "`" << ref.name() << "`, ";
	}
	out_ << "\n";
}

inline void Interpreter::add_translated_word(OperationId command,
	FutureTranslatedWordRef&& translated)
{
	save_delayed_word(command, translated);
	print_translated_word_details(command, translated);
}

inline void Interpreter::print_end_command_details(OperationId command,
	Translator::EndCommandGeneratedCode&& end_code)
{
	for (const auto& symbol_code : end_code.defined_symbols)
	{
		print_code(command, symbol_code.second);
	}

	out_ << "\n";
	print_details("start address: ");
	out_ << end_code.start_address << "\n";
}

inline void Interpreter::add_translated_line(OperationId command, TranslatedLine&& line)
{
	update_words();

	if (line.end_code)
	{
		assert(delayed_words_.empty());
		print_end_command_details(command, std::move(*line.end_code));
	}
	else
	{
		add_translated_word(command, std::move(line.word_ref));
	}
}

inline void Interpreter::update_words()
{
	const auto end = delayed_words_.end();
	const auto ready_begin = partition(delayed_words_.begin(), end,
		[](const DelayedWords::value_type& delayed_word)
	{
		return !delayed_word.first->is_ready();
	});

	for_each(ready_begin, end,
		[&](const DelayedWords::value_type& delayed_word)
	{
		print_code(delayed_word.second,
			delayed_word.first->translated_word(),
			"(future resolved)\n");
	});

	delayed_words_.erase(ready_begin, end);
}

inline void Interpreter::print_code(OperationId command, const TranslatedWord& word,
	const char* details_text /*= nullptr*/)
{
	if (details_text)
	{
		print_details(details_text);
	}

	out_ << std::setw(3) << lines_number() << "| ";
	out_.stream << std::setw(4) << std::right << word.original_address << ": ";

	const auto prev_fill = out_.stream.fill('0');

	switch (command)
	{
	case OperationId::CON:
	case OperationId::END:
		out_.stream.fill(' ');
		out_.stream << '|' << word.value.sign() << '|' << std::setw(14)
			<< std::right << word.value.abs_value() << '|' << "\n";
		break;
	case OperationId::ALF:
        out_.stream << word.value << "\n";
		break;
	default:
		out_.stream << mix::Command{word.value} << "\n";
		break;
	}

	out_.stream.fill(prev_fill);

	++printed_commands_count_;
}

inline void Interpreter::print_details(const char* details_text)
{
	assert(details_text);
	out_ << std::setw(3) << lines_number() << "> " << details_text;
}

inline std::string_view Interpreter::prepare_line(const std::string& line)
{
	// Small hack to prevent working with dead strings later
	// (Parser works with `std::string_view` and `Translator` gives 
	// unresolved symbol names that refer to parsed string)
	// #TODO: think is there better way to deal with it
	code_lines_.push_back(line);
	return code_lines_.back();
}

inline OperationId Interpreter::LineOperationId(const parse::LineParser& parser)
{
	const auto op_parser = parser.operation_parser();
	if (op_parser)
	{
		return op_parser->operation().id();
	}
	return OperationId::Unknown;
}

inline void Interpreter::translate_line(const std::string& str)
{
	if (core::Trim(str).empty())
	{
		return;
	}

	const auto line = prepare_line(str);
	parse::LineParser parser;
	const auto pos = parser.parse_stream(line);
	if (parse::IsInvalidStreamPosition(pos))
	{
		throw MixalException{"parse error"};
	}

	add_translated_line(
		LineOperationId(parser),
		TranslateLine(translator_, parser));
}

inline void TranslateStream(Interpreter& interpreter, std::istream& in)
{
	std::string line;
	while (getline(in, line))
	{
		HandleAnyException([&]()
		{
			interpreter.translate_line(line);
		});
	}
}

inline int RunInterpreter(Options options)
{
	Interpreter interpreter{std::cout, !options.hide_details};
	if (options.file_name.empty())
	{
		TranslateStream(interpreter, std::cin);
	}
	else
	{
		std::ifstream input_file(options.file_name);
		TranslateStream(interpreter, input_file);
	}
	return interpreter.printed_commands_count();
}

} // namespace mixal

