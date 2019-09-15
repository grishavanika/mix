#pragma once
#include <mixal/types.h>

#include <vector>
#include <string>
#include <sstream>

namespace mix
{
    class Computer;
} // namespace mix

struct WordWithSource
{
    mixal::TranslatedWord translated;
    int line_id = -1;
    mixal::OperationId operation_id = mixal::OperationId::Unknown;
    std::string line;
};

struct ProgramWithSource
{
    std::vector<WordWithSource> commands;
    int start_address = -1;
};

ProgramWithSource LoadProgramFromSourceFile(
    const std::string& file_name, mix::Computer* mix);

struct Debugger
{
    ProgramWithSource program_;

    std::stringstream device18_;
    std::vector<int> breakpoints_;
    int executed_instructions_count = 0;

    bool has_breakpoint(int address) const;
    void add_breakpoint(int address);
    void remove_breakpoint(int address);
};

