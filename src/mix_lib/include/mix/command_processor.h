#pragma once
#include <mix/config.h>
#include <mix/registers.h>
#include <mix/byte.h>

#include <array>

namespace mix {

class Computer;
class Command;

class MIX_LIB_EXPORT CommandProcessor
{
public:
	explicit CommandProcessor(Computer& mix);

	void process(const Command& command);

    const Computer& mix() const { return mix_; }
    Word do_load(const Command& command,
        bool reverse_sorce_sign = false) const;

private:
	void set_rax(const RAX& rax);

	const Word& memory(const Command& command) const;
	int indexed_address(const Command& command) const;
	int indexed_address(int address, std::size_t index) const;

	void do_store(const Register& r, const Command& command);

	Register do_add(Register r, const WordValue& value);
	Register do_safe_add_without_overflow_check(Sign original_sign, int value, int prev_value) const;

	Register do_enter(WordValue value, const Command& command) const;
	Register do_enter_negative(WordValue value, const Command& command) const;

	ComparisonIndicator do_compare(const Register& r, const Command& command) const;

	void do_jump(const Register& r, const Command& command);

	// #TODO: make statefull functions to return created value
	// (if this is possible/make sense)

	DeviceBlockId device_block_id(DeviceId device_id) const;

private:
	void nop(const Command& command);

	void lda(const Command& command);
	void ld1(const Command& command);
	void ld2(const Command& command);
	void ld3(const Command& command);
	void ld4(const Command& command);
	void ld5(const Command& command);
	void ld6(const Command& command);
	void ldx(const Command& command);

	void ldan(const Command& command);
	void ld1n(const Command& command);
	void ld2n(const Command& command);
	void ld3n(const Command& command);
	void ld4n(const Command& command);
	void ld5n(const Command& command);
	void ld6n(const Command& command);
	void ldxn(const Command& command);

	void sta(const Command& command);
	void st1(const Command& command);
	void st2(const Command& command);
	void st3(const Command& command);
	void st4(const Command& command);
	void st5(const Command& command);
	void st6(const Command& command);
	void stx(const Command& command);
	void stz(const Command& command);
	void stj(const Command& command);

	void add(const Command& command);
	void sub(const Command& command);

	void mul(const Command& command);
	void div(const Command& command);

	void in(const Command& command);
	void out(const Command& command);
	void ioc(const Command& command);
	void jred(const Command& command);
	void jbus(const Command& command);

	void enta_group(const Command& command);
	void entx_group(const Command& command);
	void enti_group(std::size_t index, const Command& command);
	
	void ent1_group(const Command& command);
	void ent2_group(const Command& command);
	void ent3_group(const Command& command);
	void ent4_group(const Command& command);
	void ent5_group(const Command& command);
	void ent6_group(const Command& command);

	void cmpa(const Command& command);
	void cmpx(const Command& command);
	void cmp1(const Command& command);
	void cmp2(const Command& command);
	void cmp3(const Command& command);
	void cmp4(const Command& command);
	void cmp5(const Command& command);
	void cmp6(const Command& command);

	void jmp_flags_group(const Command& command);
	void jmp_ra_group(const Command& command);
	void jmp_rx_group(const Command& command);
	void jmp_ri1_group(const Command& command);
	void jmp_ri2_group(const Command& command);
	void jmp_ri3_group(const Command& command);
	void jmp_ri4_group(const Command& command);
	void jmp_ri5_group(const Command& command);
	void jmp_ri6_group(const Command& command);

	void shift_group(const Command& command);

	void move(const Command& command);

	void convert_or_halt_group(const Command& command);

	void ra_shift(int shift);
	void rax_shift(int shift, bool cyclic);

	Register num() const;
	void char_impl();

private:
	Computer& mix_;

	using CommandAction = void (CommandProcessor::*)(const Command& command);
	static const std::array<CommandAction, Byte::k_values_count> k_command_actions;
};

} // namespace mix

