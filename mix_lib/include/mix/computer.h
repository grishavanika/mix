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
	const Register& rx() const;

	const IndexRegister& ri1() const;
	const IndexRegister& ri2() const;
	const IndexRegister& ri3() const;
	const IndexRegister& ri4() const;
	const IndexRegister& ri5() const;
	const IndexRegister& ri6() const;

private:
	void on_nop(const Command& command);
	void on_lda(const Command& command);
	void on_ld1(const Command& command);
	void on_ld2(const Command& command);
	void on_ld3(const Command& command);
	void on_ld4(const Command& command);
	void on_ld5(const Command& command);
	void on_ld6(const Command& command);
	void on_ldx(const Command& command);

	void on_sta(const Command& command);

private:
	void load_register(Register& r, const Command& command);
	void load_index_register(std::size_t index, const Command& command);

private:
	const Word& memory_with_index(int address, size_t index) const;
	Word& memory_with_index(int address, size_t index);
	const Word& memory_at(int address) const;

	const IndexRegister& index_register(size_t index) const;
	IndexRegister& index_register(size_t index);

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
