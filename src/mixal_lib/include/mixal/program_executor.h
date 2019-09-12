#pragma once
#include <mix/computer.h>
#include <mixal/line_translator.h>
#include <mixal/program_loader.h>

#include <istream>

#include <cassert>

namespace mixal {

inline int ExecuteProgram(const TranslatedProgram& program)
{
	if (program.start_address < 0)
	{
		return -1;
	}

	mix::Computer computer;
    LoadProgram(computer, program);
	return computer.run();
}

inline TranslatedProgram TranslateProgram(std::istream& in)
{
    Translator translator;
    LinesTranslator lines_translator{translator};
    std::vector<TranslatedLine> lines;

    std::string str;
    while (getline(in, str))
    {
        lines.push_back(lines_translator.translate(str));
        if (lines.back().end_code)
        {
            break;
        }
    }

    TranslatedProgram program;
    program.commands.reserve(lines.size());
    for (TranslatedLine& line : lines)
    {
        if (line.word_ref)
        {
            assert(line.word_ref->is_ready());
            program.commands.push_back(line.word_ref->translated_word());
        }
        else if (line.end_code)
        {
            program.start_address = line.end_code->start_address;
            for (const auto& end_symbols : line.end_code->defined_symbols)
            {
                program.commands.push_back(end_symbols.second);
            }
        }
    }
	return program;
}

} // namespace mixal
