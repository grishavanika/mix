#pragma once

#include <core/optional.h>

#include <cxxopts.hpp>

#include <fstream>

struct Options
{
	cxxopts::Options raw_options;
	bool execute{false};
	bool hide_details{false};
	bool show_help{false};
	std::optional<std::ifstream> input_file{};

	Options(cxxopts::Options options)
		: raw_options{std::move(options)}
	{
	}
};

inline cxxopts::Options CreateOptions()
{
	cxxopts::Options options{"mixal", "MIX interpreter/emulator"};
	options.add_options()
		("h,help",			"Show this help and exit")
		("x,hide-details",	"Hide additional information during translating")
		("e,execute",		"Compile and execute file")
		("f,file",			"File that contains MIXAL code", cxxopts::value<std::string>());
	return options;
}

inline Options ParseOptions(int argc, char* argv[])
{
	Options parsed{CreateOptions()};
	auto options = parsed.raw_options.parse(argc, argv);

	parsed.execute = (options.count("execute") > 0);
	parsed.hide_details = (options.count("hide-details") > 0);
	parsed.show_help = (options.count("help") > 0);
	const auto& file_name_option = options["file"];

	if (file_name_option.count() > 0)
	{
		std::ifstream file_input{file_name_option.as<std::string>()};
		if (file_input)
		{
			parsed.input_file = std::move(file_input);
		}
	}
	return parsed;
}

