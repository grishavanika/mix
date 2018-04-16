#include <mixal_format/stream_formatter.h>

#include <iostream>
#include <fstream>
#include <string>

#if defined(_MSC_VER) && defined(__clang__)
#  pragma clang diagnostic push
// Comes from <regex>, -Wno-sign-compare on command line does not help
// 
// comparison of integers of different signs
#  pragma clang diagnostic ignored "-Wsign-compare"
#endif

#include <cxxopts.hpp>

#if defined(_MSC_VER) && defined(__clang__)
#  pragma clang diagnostic pop
#endif

using namespace mixal_format;

namespace
{
	cxxopts::Options CreateOptions()
	{
		cxxopts::Options options{"mixal_format", "MIXAL formatter"};
		options.add_options()
			("h,help",	"Show this help and exit")
			("f,file",	"Input MIXAL source file", cxxopts::value<std::string>())
			("m,mdk",	R"(Format to "GNU MIX Development Kit" syntax)")
			("u,uppercase", "Make all symbols upper-case");
		return options;
	}

	struct Options
	{
		FormatOptions format;
		std::string file;
		bool show_help = false;
	};

	Options OptionsFromCommandLine(const cxxopts::ParseResult& cmd)
	{
		Options options;

		const auto file_opt = cmd["file"];
		if (file_opt.count() == 1)
		{
			options.file = file_opt.as<std::string>();
		}
		options.show_help = (cmd.count("help") > 0);
		options.format.mdk_compatible = (cmd.count("mdk") > 0);
		options.format.make_all_uppercase = (cmd.count("uppercase") > 0);

		return options;
	}
}

int main(int argc, char* argv[])
{
	try
	{
		auto cmd_args = CreateOptions();
		Options options = OptionsFromCommandLine(cmd_args.parse(argc, argv));
		if (options.show_help)
		{
			std::cout << cmd_args.help() << '\n';
			return 0;
		}

		if (options.format.mdk_compatible)
		{
			options.format.title_comment =
				"Formatted to GNU MIX Development Kit syntax";
		}

		std::ifstream input{options.file};
		FormatStream(input, std::cout, options.format);
		return 0;
	}
	catch (const std::exception& e)
	{
		std::cout << "Exception: " << e.what() << '\n';
		return -1;
	}
}
