#include <mixui/command_help.h>
#include <mix/computer.h>
#include <mix/command.h>
#include <mix/command_processor.h>
#include <mix/word.h>
#include <mixal/operation_info.h>

#include <ostream>
#include <sstream>
#include <iomanip>

#include <cassert>
#include <cstdint>

using namespace mix;

/*static*/ const std::array<
    CommandHelp::CommandAction,
    Byte::k_values_count>
CommandHelp::k_command_actions = {{
    /*00*/&CommandHelp::noop,
    /*01*/&CommandHelp::add,
    /*02*/&CommandHelp::sub,
    /*03*/&CommandHelp::mul,
    /*04*/&CommandHelp::div,
    /*05*/&CommandHelp::convert_or_halt_group,
    /*06*/&CommandHelp::shift_group,
    /*07*/&CommandHelp::move,
    /*08*/&CommandHelp::lda,
    /*09*/&CommandHelp::ld1,
    /*10*/&CommandHelp::ld2,
    /*11*/&CommandHelp::ld3,
    /*12*/&CommandHelp::ld4,
    /*13*/&CommandHelp::ld5,
    /*14*/&CommandHelp::ld6,
    /*15*/&CommandHelp::ldx,
    /*16*/&CommandHelp::ldan,
    /*17*/&CommandHelp::ld1n,
    /*18*/&CommandHelp::ld2n,
    /*19*/&CommandHelp::ld3n,
    /*20*/&CommandHelp::ld4n,
    /*21*/&CommandHelp::ld5n,
    /*22*/&CommandHelp::ld6n,
    /*23*/&CommandHelp::ldxn,
    /*24*/&CommandHelp::sta,
    /*25*/&CommandHelp::st1,
    /*26*/&CommandHelp::st2,
    /*27*/&CommandHelp::st3,
    /*28*/&CommandHelp::st4,
    /*29*/&CommandHelp::st5,
    /*30*/&CommandHelp::st6,
    /*31*/&CommandHelp::stx,
    /*32*/&CommandHelp::stj,
    /*33*/&CommandHelp::stz,
    /*34*/&CommandHelp::jbus,
    /*35*/&CommandHelp::ioc,
    /*36*/&CommandHelp::in,
    /*37*/&CommandHelp::out,
    /*38*/&CommandHelp::jred,
    /*39*/&CommandHelp::jmp_flags_group,
    /*40*/&CommandHelp::jmp_ra_group,
    /*41*/&CommandHelp::jmp_ri1_group,
    /*42*/&CommandHelp::jmp_ri2_group,
    /*43*/&CommandHelp::jmp_ri3_group,
    /*44*/&CommandHelp::jmp_ri4_group,
    /*45*/&CommandHelp::jmp_ri5_group,
    /*46*/&CommandHelp::jmp_ri6_group,
    /*47*/&CommandHelp::jmp_rx_group,
    /*48*/&CommandHelp::enta_group,
    /*49*/&CommandHelp::ent1_group,
    /*50*/&CommandHelp::ent2_group,
    /*51*/&CommandHelp::ent3_group,
    /*52*/&CommandHelp::ent4_group,
    /*53*/&CommandHelp::ent5_group,
    /*54*/&CommandHelp::ent6_group,
    /*55*/&CommandHelp::entx_group,
    /*56*/&CommandHelp::cmpa,
    /*57*/&CommandHelp::cmp1,
    /*58*/&CommandHelp::cmp2,
    /*59*/&CommandHelp::cmp3,
    /*60*/&CommandHelp::cmp4,
    /*61*/&CommandHelp::cmp5,
    /*62*/&CommandHelp::cmp6,
    /*63*/&CommandHelp::cmpx
    }};

namespace
{
    void DumpWordInfo(
        const char* title
        , std::ostream& o
        , const Word& w
        , const WordField& f
        , bool show_field = true)
    {
        o << title << ": ";
        if (show_field)
        {
            o << w << "; field ("
                << f.left() << ":" << f.right() << ")"
                << "; value: " << w.value();
        }
        else
        {
            o << w << "; value: " << w.value();
        }
        o << "\n";
    }

    void DumpLoadOperationInfo(
        const char* title
        , std::ostream& o
        , const Word& w
        , const WordField& source
        , const WordField& dest
        , bool show_field = true)
    {
        const auto f = (source == Word::MaxField()) ? source : dest;
        DumpWordInfo(title, o, w, f, show_field);
    }
} // namespace

CommandHelp::CommandHelp(const CommandProcessor& processor)
    : processor_(processor)
{
}

void CommandHelp::describe_command(const Command& command, std::ostream& out) const
{
    auto callback = k_command_actions[command.id()];
    assert(callback && "Invalid/not implemented command help");

    const auto name = mixal_parse::OperationIdToString(
        mixal::QueryOperationInfo(command).id);
    out << name << ": " << command << "\n";

    // #XXX: temporary. Callbacks need to be const.
    // Lazy to mark them all now
    (const_cast<CommandHelp*>(this)->*callback)(command, out);
}

std::string CommandHelp::describe_command(const Command& command) const
{
    std::ostringstream ss;
    describe_command(command, ss);
    return std::move(ss).str();
}

void CommandHelp::describe_address(int address, std::ostream& out) const
{
    if ((address < 0)
        || (address >= static_cast<int>(mix::Computer::k_memory_words_count)))
    {
        return;
    }
    const Command cmd(processor_.mix().memory(address));
    describe_command(cmd, out);
}

std::string CommandHelp::describe_address(int address) const
{
    std::ostringstream ss;
    describe_address(address, ss);
    return std::move(ss).str();
}

int CommandHelp::indexed_address(const Command& command) const
{
    const std::size_t index = command.address_index();
    if (index == 0)
    {
        return command.address();
    }
    return command.address() + processor_.mix().ri(index).value();
}

void CommandHelp::noop(const Command& /*command*/, std::ostream& /*out*/)
{
}

void CommandHelp::do_ldax(const char* name, const Register& current
    , const Command& command, std::ostream& out)
{
    // Short description
    const bool is_partial_load = !(command.word_field() == Word::MaxField());
    const int source_address = indexed_address(command);
    const Word source_content = processor_.mix().memory(source_address);
    const auto source_field = command.word_field();
    const auto dest_field = source_field.shift_bytes_right();

    out << "Load "
        << (is_partial_load ? "part of" : "whole")
        << " word from memory address "
        << source_address
        << " to register " << name << "."
        << "\n\n";

    // Long description
    const std::size_t index = command.address_index();
    out << "Memory address is taken from command's address"
        << " field (+- AA), bytes "
        << WordField(0, 2) << ". Current value is: "
        << command.address() << ".";
    if (index != 0)
    {
        const WordValue ri = processor_.mix().ri(index).value();
        out << " Additionally, value from index register " << index
            << " (I" << index << ") is taken into account."
            << " Current I" << index << " register value is: " << ri << "."
            << " Final value is: " << command.address() << " + " << ri
            << " = " << source_address << ".";
    }
    else
    {
        out << " Index register is ignored (byte 3 of the command is 0)" << ".";
    }
    out << "\n\n";
    
    if (is_partial_load)
    {
        out << "Partial word will be copied."
            << " Source word field (found in 4th byte of the command: "
            << command.field() << ") is "
            << command.word_field() << ".";
        if (source_field.bytes_count() != Word::k_bytes_count)
        {
            out << " Unspecified bytes of " << name << " register"
                << " will be zeroed" << ".";
        }
        if (source_field.has_only_sign())
        {
            out << " Only sign will be copied from"
                << " source memory address ("
                << source_content.sign() << ")" << ".";
        }
        else
        {
            if (source_field.bytes_count() != Word::k_bytes_count)
            {
                out << " Source word field is shifted to the right of "
                    << name << " register, destination word field: "
                    << dest_field << ".";
            }
            if (source_field.includes_sign())
            {
                out << " Sign will be copied from"
                    << " source memory address ("
                    << source_content.sign() << ")" << ".";
            }
            else
            {
                out << " Sign of the source memory address"
                    << " is ignored and will be set to "
                    << Sign::Positive << ".";
            }
        }
        out << "\n\n";
    }

    DumpWordInfo("(1)", out, current, source_field, false/*do not show field*/);
    DumpWordInfo("(2)", out, source_content, source_field);
    DumpLoadOperationInfo("(3)", out, processor_.do_load(command), source_field, dest_field);

    out << "\n"
        << "Where:"
        << "\n";
    out << "(1): " << "Register " << name << " before load" << "\n";
    out << "(2): " << "Memory address " << source_address << " content" << "\n";
    out << "(3): " << "Register " << name << " after load" << "\n";
}

void CommandHelp::lda(const Command& command, std::ostream& out)
{
    do_ldax("A", processor_.mix().ra(), command, out);
}

void CommandHelp::ldx(const Command& command, std::ostream& out)
{
    do_ldax("X", processor_.mix().rx(), command, out);
}
