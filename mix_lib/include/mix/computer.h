#include <mix/registers.h>
#include <mix/general_enums.h>

namespace mix {

class Computer
{
public:
	static constexpr std::size_t k_index_registers_count = 6;
	static constexpr std::size_t k_memory_words_count = 4000;

	explicit Computer();

private:


private:
	Register ra_;
	Register rx_;
	std::array<IndexRegister, k_index_registers_count> rindexes_;
	AddressRegister rj_;

	ComparisonIndicator comparison_state_;
	OverflowFlag overflow_flag_;

	std::array<Word, k_memory_words_count> memory_;
};


} // namespace mix
