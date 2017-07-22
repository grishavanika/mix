#include <mix/registers.h>
#include <mix/general_enums.h>
#include <mix/command.h>

namespace mix {

class Computer
{
public:
	static constexpr std::size_t k_index_registers_count = 6;
	static constexpr std::size_t k_memory_words_count = 4000;
	static constexpr std::size_t k_commands_count = 64;

	explicit Computer();

	void execute(const Command& command);

	void set_memory(std::size_t address, const Word& value);

	const Register& ra() const;

private:
	void on_nop(const Command& command);
	void on_lda(const Command& command);

private:
	const Word& memory(int address, size_t index) const;
	const Word& memory(int address) const;

	int index_value(size_t index) const;

private:
	Register ra_;
	Register rx_;
	std::array<IndexRegister, k_index_registers_count> rindexes_;
	AddressRegister rj_;

	ComparisonIndicator comparison_state_;
	OverflowFlag overflow_flag_;

	std::array<Word, k_memory_words_count> memory_;

	using CommandAction = void (Computer::*)(const Command& command);
	static const std::array<CommandAction, k_commands_count> k_command_actions;
};


} // namespace mix
