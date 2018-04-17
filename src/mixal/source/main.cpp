#include <mixal/interpreter.h>
#include <mixal/program_executor.h>

using namespace mixal;

namespace
{
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

