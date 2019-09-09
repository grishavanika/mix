#pragma once
#include <mix/computer.h>
#include <mixal/line_translator.h>
#include <mixal/program_loader.h>

#include <istream>

namespace mixal {

inline int ExecuteProgram(const TranslatedProgram& program)
{
	if (!program.completed)
	{
		return -1;
	}

	mix::Computer computer;
    LoadProgram(computer, program);
	return computer.run();
}

inline ProgramTranslator TranslateProgram(std::istream& in)
{
	Translator translator;
	ProgramTranslator program_translator{translator};
	std::string line;
	while (getline(in, line))
	{
		const auto status = program_translator.translate_line(line);
		if (status == ProgramTranslator::Status::Completed)
		{
			break;
		}
	}

	return program_translator;
}

} // namespace mixal
