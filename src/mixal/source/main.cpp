#include <mixal/line_translator.h>
#include <mixal/translator.h>
#include <mixal/exceptions.h>
#include <mixal/program_executor.h>

#include <mixal_parse/line_parser.h>

#include <mix/exceptions.h>
#include <mix/command.h>
#include <mix/computer.h>

#include <core/string.h>

#include <string>
#include <iostream>
#include <iomanip>
#include <list>

using namespace mixal;
using namespace mixal_parse;
using namespace mix;

struct OptionalOStream
{
	std::ostream& stream;
	bool enabled;

	OptionalOStream(std::ostream& out, bool enable)
		: stream{out}
		, enabled{enable}
	{
	}
};

template<typename T>
OptionalOStream& operator<<(OptionalOStream& out, const T& value)
{
	if (out.enabled)
	{
		out.stream << value;
	}
	return out;
}

struct Interpreter
{
	using DelayedWords = FlatMap<FutureTranslatedWordRef, OperationId>;

	OptionalOStream out;
	Translator translator;
	DelayedWords delayed_words;

	std::list<std::string> code_lines;

	Interpreter(std::ostream& out, bool show_details)
		: out{out, show_details}
		, translator{}
		, delayed_words{}
		, code_lines{}
	{
	}

	std::size_t lines_number() const
	{
		return code_lines.size();
	}

	void save_delayed(OperationId command, FutureTranslatedWordRef translated)
	{
		if (translated && !translated->is_ready())
		{
			delayed_words.push_back(std::make_pair(std::move(translated), command));
		}
	}

	void print_translated_word_details(OperationId command, FutureTranslatedWordRef translated)
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
			out << "`" << ref.name() << "`, ";
		}
		out << "\n";
	}

	void add_translated_word(OperationId command, FutureTranslatedWordRef&& translated)
	{
		save_delayed(command, translated);
		print_translated_word_details(command, translated);
	}

	void print_end_command_details(OperationId command, Translator::EndCommandGeneratedCode&& end_code)
	{
		for (const auto& symbol_code : end_code.defined_symbols)
		{
			print_code(command, symbol_code.second);
		}

		out << "\n";
		print_details("start address: ");
		out << end_code.start_address << "\n";
	}

	void add_translated_line(OperationId command, TranslatedLine&& line)
	{
		update_words();

		if (line.end_code)
		{
			assert(delayed_words.empty());
			print_end_command_details(command, std::move(*line.end_code));
		}
		else
		{
			add_translated_word(command, std::move(line.word_ref));
		}
	}

	void update_words()
	{
		const auto end = delayed_words.end();
		const auto ready_begin = partition(delayed_words.begin(), end,
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

		delayed_words.erase(ready_begin, end);
	}

	void print_code(OperationId command, const TranslatedWord& word,
		const char* details_text = nullptr)
	{
		if (details_text)
		{
			print_details(details_text);
		}

		out << std::setw(3) << lines_number() << "| ";
		out.stream << std::setw(4) << std::right << word.original_address << ": ";

		const auto prev_fill = out.stream.fill('0');

		switch (command)
		{
		case OperationId::CON:
		case OperationId::END:
			out.stream.fill(' ');
			out.stream << '|' << word.value.sign() << '|' << std::setw(14)
				<< std::right << word.value.abs_value() << '|' << "\n";
			break;
		case OperationId::ALF:
			out.stream << word.value << "\n";
			break;
		default:
			out.stream << Command{word.value} << "\n";
			break;
		}
		
		out.stream.fill(prev_fill);
	}

	void print_details(const char* details_text)
	{
		out << std::setw(3) << lines_number()
			<< "> " << details_text;
	}

	std::string_view prepare_line(const std::string& line)
	{
		// Small hack to prevent working with dead strings later
		// (Parser works with `std::string_view` and `Translator` gives 
		// unresolved symbol names that refer to parsed string)
		// #TODO: think is there better way to deal with it
		code_lines.push_back(line);
		return code_lines.back();
	}
};

OperationId LineOperationId(const LineParser& parser)
{
	if (parser.operation_parser())
	{
		return parser.operation_parser()->operation().id();
	}
	return OperationId::Unknown;
}

void TranslateLine(Interpreter& interpreter, const std::string& str)
{
	if (core::Trim(str).empty())
	{
		return;
	}

	const auto line = interpreter.prepare_line(str);
	LineParser parser;
	const auto pos = parser.parse_stream(line);
	if (IsInvalidStreamPosition(pos))
	{
		throw std::runtime_error{"parse error"};
	}

	interpreter.add_translated_line(
		LineOperationId(parser),
		TranslateLine(interpreter.translator, parser));
}

template<typename Callable>
void HandleAnyException(Callable callable)
{
	try
	{
		callable();
	}
	catch (const mixal::MixalException& mixal_error)
	{
		std::cerr << "[MIXAL] Error: " << mixal_error.what() << "\n";
	}
	catch (const mix::MixException& mix_error)
	{
		std::cerr << "[MIX] Error: " << mix_error.what() << "\n";
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << "\n";
	}
	catch (...)
	{
		std::cerr << "Error: " << "UNKNOWN" << "\n";
	}
}

void TranslateStream(Interpreter& interpreter, std::istream& in)
{
	std::string line;
	while (getline(in, line))
	{
		HandleAnyException([&]()
		{
			TranslateLine(interpreter, line);
		});
	}
}

#include <fstream>

int RunInterpreter(int argc, char* /*argv*/[])
{
	const bool hide_details = (argc > 1);
	Interpreter interpreter{std::cout, !hide_details};
#if (0)
	std::ifstream in{R"(test.mixal)"};
	TranslateStream(interpreter, in);
#else
	TranslateStream(interpreter, std::cin);
#endif
	return 0;
}

int ExecuteProgram(const TranslatedProgram& program)
{
	if (!program.completed)
	{
		return -1;
	}

	Computer computer;
	LoadProgram(computer, program);
	computer.run();
	return 0;
}

int RunProgram(int /*argc*/, char* /*argv*/[])
{
#if (0)
	std::ifstream file_input{R"(program_primes.mixal)"};
	std::istream& in{file_input};
#else
	std::istream& in{std::cin};
#endif

	Translator translator;
	ProgramTranslator program_translator{translator};
	std::string line;

	HandleAnyException([&]()
	{
		while (getline(in, line))
		{
			const auto status = program_translator.translate_line(line);
			if (status == ProgramTranslator::Status::Completed)
			{
				break;
			}
		}

		(void)ExecuteProgram(program_translator.program());
	});

	return 0;
}

int main(int argc, char* argv[])
{
	const bool run_interpreter = false;
	if (run_interpreter)
	{
		return RunInterpreter(argc, argv);
	}
	else
	{
		return RunProgram(argc, argv);
	}
}

