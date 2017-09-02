#pragma once
#include <mix/computer.h>

#include <mixal/line_translator.h>

#include <mixal/program_loader.h>

#include <mixal/exceptions_handler.h>

namespace mixal {

inline int ExecuteProgram(const TranslatedProgram& program)
{
	if (!program.completed)
	{
		return -1;
	}

	mix::Computer computer;
	LoadProgram(computer, program);
	computer.run();
	return 0;
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

inline int RunProgram(Options options)
{
	if (!options.input_file)
	{
		return -1;
	}

	int status = -1;
	HandleAnyException([&]()
	{
		auto program_translator = TranslateProgram(*options.input_file);
		status = ExecuteProgram(program_translator.program());
	});

	return status;
}

} // namespace mixal
