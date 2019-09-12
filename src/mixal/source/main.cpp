#include <mixal/interpreter.h>
#include <mixal/program_executor.h>
#include <mixal/exceptions_handler.h>
#include <mixal/mdk_program_loader.h>

#include <fstream>

using namespace mixal;

namespace
{
    int RunProgram(Options options)
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
                program = ParseProgramFromMDKStream(input);
            }
            else
            {
                std::ifstream input(options.file_name);
                program = TranslateProgram(input);
            }

            commands_count = ExecuteProgram(program);
        });

        return commands_count;
    }

	void RunWithOptions(Options options)
	{
		if (options.show_help)
		{
			std::cout << options.raw_options.help() << '\n';
			return;
		}

		if (options.execute)
		{
			const int commands_count = RunProgram(std::move(options));
			std::cout << "Executed commands count: " << commands_count << '\n';
		}
		else if (options.interactive_compile)
		{
			const int commands_count = RunInterpreter(std::move(options));
			std::cout << "Commands count: " << commands_count << '\n';
		}
	}
} // namespace

int main(int argc, char* argv[])
{
	try
	{
		RunWithOptions(ParseOptions(argc, argv));
		return 0;
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return -1;
	}
}

