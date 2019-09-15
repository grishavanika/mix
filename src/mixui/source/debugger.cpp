#include <mixui/debugger.h>

#include <mixal/translator.h>
#include <mixal/line_translator.h>

#include <mix/computer.h>

#include <fstream>

static void LoadProgram(const ProgramWithSource& program, mix::Computer* computer)
{
    for (const auto& word : program.commands)
    {
        const int address = word.translated.original_address;
        if (address >= 0)
        {
            computer->set_memory(
                word.translated.original_address
                , word.translated.value);
        }
    }
    computer->set_next_address(program.start_address);
}

ProgramWithSource LoadProgramFromSourceFile(
    const std::string& file_name, mix::Computer* mix)
{
    std::ifstream in(file_name);
    mixal::Translator translator;
    mixal::LinesTranslator lines_translator(translator);
    std::vector<mixal::TranslatedLine> lines;
    std::vector<std::string> strs;

    try
    {
        std::string str;
        while (getline(in, str))
        {
            lines.push_back(lines_translator.translate(str));
            strs.push_back(std::move(str));
            if (lines.back().end_code)
            {
                break;
            }
        }
    }
    catch (const std::exception&)
    {
        return ProgramWithSource();
    }

    ProgramWithSource program;

    for (int line_id = 0, count = static_cast<int>(lines.size()); line_id < count; ++line_id)
    {
        auto word_ref = lines[line_id].word_ref;
        auto end_code = lines[line_id].end_code;
        auto id = lines[line_id].operation_id;
        auto str = strs[line_id];
        if (word_ref)
        {
            assert(word_ref->is_ready());
            WordWithSource word;
            word.translated = word_ref->translated_word();
            word.line_id = line_id + 1;
            word.operation_id = id;
            word.line = str;
            program.commands.push_back(word);
        }
        else if (end_code)
        {
            program.start_address = end_code->start_address;
            for (auto& end_symbols : end_code->defined_symbols)
            {
                WordWithSource word;
                word.translated = end_symbols.second;
                word.line_id = line_id + 1;
                word.operation_id = id;
                word.line = str;
                program.commands.push_back(word);
            }
        }
        else
        {
            WordWithSource word;
            word.line_id = line_id + 1;
            word.operation_id = id;
            word.line = str;
            program.commands.push_back(word);
        }
    }

    LoadProgram(program, mix);
    return program;
}

bool Debugger::has_breakpoint(int address) const
{
    const auto it = std::find(
        std::cbegin(breakpoints_)
        , std::cend(breakpoints_)
        , address);
    return (it != std::cend(breakpoints_));
}

void Debugger::add_breakpoint(int address)
{
    assert(address >= 0);
    breakpoints_.push_back(address);
}

void Debugger::remove_breakpoint(int address)
{
    assert(address >= 0);
    const auto it = std::find(
        std::begin(breakpoints_)
        , std::end(breakpoints_)
        , address);
    if (it != std::end(breakpoints_))
    {
        breakpoints_.erase(it);
    }
}

