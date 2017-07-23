#include <mix/registers.h>
#include <mix/general_enums.h>

namespace mix {

class CommandProcessor;
class Command;

class Computer
{
public:
	static constexpr std::size_t k_index_registers_count = 6;
	static constexpr std::size_t k_memory_words_count = 4000;

	explicit Computer();

	void execute(const Command& command);

	void set_memory(std::size_t address, const Word& value);
	const Word& memory(std::size_t address) const;

	const Register& ra() const;
	void set_ra(const Register& ra);

	const Register& rx() const;
	void set_rx(const Register& rx);

	const IndexRegister& ri(std::size_t index) const;
	void set_ri(std::size_t index, const IndexRegister& ri);

	OverflowFlag overflow_flag() const;
	bool has_overflow() const;
	void set_overflow();

private:
	const Word& memory_with_index(int address, size_t index) const;
	Word& memory_with_index(int address, size_t index);
	const Word& memory_at(int address) const;

	const IndexRegister& index_register(size_t index) const;
	IndexRegister& index_register(size_t index);

private:
	// #TODO: make this relationship thru interface
	friend class CommandProcessor;

	Register ra_;
	Register rx_;
	std::array<IndexRegister, k_index_registers_count> rindexes_;
	AddressRegister rj_;

	ComparisonIndicator comparison_state_;
	OverflowFlag overflow_flag_;

	std::array<Word, k_memory_words_count> memory_;
};


} // namespace mix
