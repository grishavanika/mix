#pragma once
#include <mix/config.h>
#include <mix/byte.h>

#include <array>
#include <iosfwd>
#include <string>

namespace mix
{

    class Command;
    class Register;
    class CommandProcessor;

    class MIX_LIB_EXPORT CommandHelp
    {
    public:
        explicit CommandHelp(const CommandProcessor& processor);

        void describe_command(const Command& command, std::ostream& out) const;
        std::string describe_command(const Command& command) const;

        void describe_address(int address, std::ostream& out) const;
        std::string describe_address(int address) const;

    private:
        int indexed_address(const Command& command) const;
        void do_ldax(const char* name, const Register& current
            , const Command& command, std::ostream& out);

    private:
        void noop(const Command& command, std::ostream& out);
        void lda(const Command& command, std::ostream& out);
        void ld1(const Command& command, std::ostream& out) { (void)command; (void)out; }
        void ld2(const Command& command, std::ostream& out) { (void)command; (void)out; }
        void ld3(const Command& command, std::ostream& out) { (void)command; (void)out; }
        void ld4(const Command& command, std::ostream& out) { (void)command; (void)out; }
        void ld5(const Command& command, std::ostream& out) { (void)command; (void)out; }
        void ld6(const Command& command, std::ostream& out) { (void)command; (void)out; }
        void ldx(const Command& command, std::ostream& out);

        void ldan(const Command& command, std::ostream& out) { (void)command; (void)out; }
        void ld1n(const Command& command, std::ostream& out) { (void)command; (void)out; }
        void ld2n(const Command& command, std::ostream& out) { (void)command; (void)out; }
        void ld3n(const Command& command, std::ostream& out) { (void)command; (void)out; }
        void ld4n(const Command& command, std::ostream& out) { (void)command; (void)out; }
        void ld5n(const Command& command, std::ostream& out) { (void)command; (void)out; }
        void ld6n(const Command& command, std::ostream& out) { (void)command; (void)out; }
        void ldxn(const Command& command, std::ostream& out) { (void)command; (void)out; }

        void sta(const Command& command, std::ostream& out) { (void)command; (void)out; }
        void st1(const Command& command, std::ostream& out) { (void)command; (void)out; }
        void st2(const Command& command, std::ostream& out) { (void)command; (void)out; }
        void st3(const Command& command, std::ostream& out) { (void)command; (void)out; }
        void st4(const Command& command, std::ostream& out) { (void)command; (void)out; }
        void st5(const Command& command, std::ostream& out) { (void)command; (void)out; }
        void st6(const Command& command, std::ostream& out) { (void)command; (void)out; }
        void stx(const Command& command, std::ostream& out) { (void)command; (void)out; }
        void stz(const Command& command, std::ostream& out) { (void)command; (void)out; }
        void stj(const Command& command, std::ostream& out) { (void)command; (void)out; }

        void add(const Command& command, std::ostream& out) { (void)command; (void)out; }
        void sub(const Command& command, std::ostream& out) { (void)command; (void)out; }

        void mul(const Command& command, std::ostream& out) { (void)command; (void)out; }
        void div(const Command& command, std::ostream& out) { (void)command; (void)out; }

        void in(const Command& command, std::ostream& out) { (void)command; (void)out; }
        void out(const Command& command, std::ostream& out) { (void)command; (void)out; }
        void ioc(const Command& command, std::ostream& out) { (void)command; (void)out; }
        void jred(const Command& command, std::ostream& out) { (void)command; (void)out; }
        void jbus(const Command& command, std::ostream& out) { (void)command; (void)out; }

        void enta_group(const Command& command, std::ostream& out) { (void)command; (void)out; }
        void entx_group(const Command& command, std::ostream& out) { (void)command; (void)out; }
        void enti_group(std::size_t index, const Command& command, std::ostream& out) { (void)index; (void)command; (void)out; }

        void ent1_group(const Command& command, std::ostream& out) { (void)command; (void)out; }
        void ent2_group(const Command& command, std::ostream& out) { (void)command; (void)out; }
        void ent3_group(const Command& command, std::ostream& out) { (void)command; (void)out; }
        void ent4_group(const Command& command, std::ostream& out) { (void)command; (void)out; }
        void ent5_group(const Command& command, std::ostream& out) { (void)command; (void)out; }
        void ent6_group(const Command& command, std::ostream& out) { (void)command; (void)out; }

        void cmpa(const Command& command, std::ostream& out) { (void)command; (void)out; }
        void cmpx(const Command& command, std::ostream& out) { (void)command; (void)out; }
        void cmp1(const Command& command, std::ostream& out) { (void)command; (void)out; }
        void cmp2(const Command& command, std::ostream& out) { (void)command; (void)out; }
        void cmp3(const Command& command, std::ostream& out) { (void)command; (void)out; }
        void cmp4(const Command& command, std::ostream& out) { (void)command; (void)out; }
        void cmp5(const Command& command, std::ostream& out) { (void)command; (void)out; }
        void cmp6(const Command& command, std::ostream& out) { (void)command; (void)out; }

        void jmp_flags_group(const Command& command, std::ostream& out) { (void)command; (void)out; }
        void jmp_ra_group(const Command& command, std::ostream& out) { (void)command; (void)out; }
        void jmp_rx_group(const Command& command, std::ostream& out) { (void)command; (void)out; }
        void jmp_ri1_group(const Command& command, std::ostream& out) { (void)command; (void)out; }
        void jmp_ri2_group(const Command& command, std::ostream& out) { (void)command; (void)out; }
        void jmp_ri3_group(const Command& command, std::ostream& out) { (void)command; (void)out; }
        void jmp_ri4_group(const Command& command, std::ostream& out) { (void)command; (void)out; }
        void jmp_ri5_group(const Command& command, std::ostream& out) { (void)command; (void)out; }
        void jmp_ri6_group(const Command& command, std::ostream& out) { (void)command; (void)out; }

        void shift_group(const Command& command, std::ostream& out) { (void)command; (void)out; }

        void move(const Command& command, std::ostream& out) { (void)command; (void)out; }

        void convert_or_halt_group(const Command& command, std::ostream& out) { (void)command; (void)out; }

    private:
        const CommandProcessor& processor_;

        using CommandAction = void (CommandHelp::*)(const Command& command, std::ostream& out);
        static const std::array<CommandAction, Byte::k_values_count> k_command_actions;
    };

} // namespace mix

