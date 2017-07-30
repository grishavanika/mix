#pragma once
#include <mix/registers.h>

#include <array>

namespace mix {

class Computer;
class Command;
class Word;
class WordValue;

class CommandProcessor
{
public:
	static constexpr std::size_t k_commands_count = 64;

	explicit CommandProcessor(Computer& mix);

	void process(const Command& command);

private:
	Register load_register(
		Register prev_value,
		const Command& command,
		bool reverse_sorce_sign = false) const;

	IndexRegister load_index_register(
		std::size_t index,
		const Command& command,
		bool reverse_sorce_sign = false) const;

	const Word& memory(const Command& command) const;
	int indexed_address(int address, std::size_t ri) const;
	int indexed_address(const Command& command) const;

	Register do_add(const WordValue& source) const;
	Register do_safe_add_without_overflow_check(int value, int prev_value) const;

	Register enter(const Command& command) const;
	Register enter_negative(const Command& command) const;

	void store_register(const Register& r, const Command& command);

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

	void enta_group(const Command& command);
	void entx_group(const Command& command);
	void enti_group(std::size_t index, const Command& command);
	
	void ent1_group(const Command& command);
	void ent2_group(const Command& command);
	void ent3_group(const Command& command);
	void ent4_group(const Command& command);
	void ent5_group(const Command& command);
	void ent6_group(const Command& command);


private:
	Computer& mix_;

	using CommandAction = void (CommandProcessor::*)(const Command& command);
	static const std::array<CommandAction, k_commands_count> k_command_actions;
};

} // namespace mix

