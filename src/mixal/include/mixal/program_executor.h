#pragma once
#include <mix/computer.h>

#include <mixal/line_translator.h>
#include <mixal/program_loader.h>
#include <mixal/mdk_program_loader.h>

#include <mixal/exceptions_handler.h>

#include <fstream>

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

inline int RunProgram(Options options)
{
	if (options.file_name.empty())
	{
		return -1;
	}

	int commands_count = -1;
	HandleAnyException([&]()
	{
		TranslatedProgram program;
		if (options.mdk_stream)
		{
			std::ifstream input(options.file_name, std::ios_base::binary);
			auto mdk_program = ParseProgramFromMDKStream(input);
			program = std::move(mdk_program);
		}
		else
		{
			std::ifstream input(options.file_name);
			auto program_translator = TranslateProgram(input);
			program = std::move(program_translator.program());
		}

		commands_count = ExecuteProgram(program);
	});

	return commands_count;
}

} // namespace mixal
