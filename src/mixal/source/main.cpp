#include <mixal/line_translator.h>
#include <mixal/translator.h>
#include <mixal/exceptions.h>

#include <mixal_parse/line_parser.h>

#include <mix/exceptions.h>
#include <mix/command.h>

#include <core/string.h>

#include <string>
#include <iostream>

using namespace mixal;
using namespace mixal_parse;
using namespace mix;

struct Interpreter
{
	std::ostream& out;
	Translator translator;
	std::vector<FutureTranslatedWordRef> words;
	// #TODO: temporary solution
	std::vector<std::string> code_lines;

	void add(FutureTranslatedWordRef&& translated)
	{
		update_words();

		if (!translated)
		{
			out << "> no code generated" << "\n";
		}
		else if (!translated->is_ready())
		{
			out << "> delaying interpretation because of: ";
			for (const auto& ref : translated->forward_references)
			{
				out << "`" << ref.name() << "`, ";
			}
			out << "\n";
			words.push_back(std::move(translated));
		}
		else
		{
			print_code(*translated);
		}
	}

	void update_words()
	{
		for (auto it = words.begin(); it != words.end(); )
		{
			if ((**it).is_ready())
			{
				print_code(**it);
				it = words.erase(it);
			}
			else
			{
				++it;
			}
		}
	}

	void print_code(const FutureTranslatedWord& word)
	{
		out << word.original_address << ": " << Command{word.value} << "\n";
	}
};

void TranslateLine(Interpreter& interpreter, const std::string& str)
{
	if (core::Trim(str).empty())
	{
		return;
	}

	interpreter.code_lines.push_back(str);
	const std::string_view line{interpreter.code_lines.back()};

	LineParser parser;
	const auto pos = parser.parse_stream(line);
	if (IsInvalidStreamPosition(pos))
	{
		throw std::runtime_error{"parse error"};
	}

	auto translated = TranslateLine(interpreter.translator, parser);
	interpreter.add(std::move(translated));
}

void TranslateStream(Interpreter& interpreter, std::istream& in)
{
	std::string line;
	while (getline(in, line))
	{
		try
		{
			TranslateLine(interpreter, line);
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
}

int main()
{
	Interpreter interpreter{std::cout};
	interpreter.code_lines.reserve(1000);
	TranslateStream(interpreter, std::cin);
}
