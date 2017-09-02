#include <mixal/interpreter.h>
#include <mixal/program_executor.h>

int HandleOptions(Options options)
{
	if (options.show_help)
	{
		std::cout << options.raw_options.help() << '\n';
		return 0;
	}

	if (options.execute)
	{
		return RunProgram(std::move(options));
	}
	else
	{
		return RunInterpreter(std::move(options));
	}
}

int main(int argc, char* argv[])
{
	try
	{
		return HandleOptions(
			ParseOptions(argc, argv));
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return -1;
	}
}

