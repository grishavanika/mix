#include <mix/computer.h>

using namespace mix;

/*static*/ const std::array<
	Computer::CommandAction,
	Computer::k_commands_count>
Computer::k_command_actions = {
	/*00*/&Computer::on_nop,
	/*01*/nullptr,
	/*02*/nullptr,
	/*03*/nullptr,
	/*04*/nullptr,
	/*05*/nullptr,
	/*06*/nullptr,
	/*07*/nullptr,
	/*08*/&Computer::on_lda,
};

Computer::Computer()
{
}

const Register& Computer::ra() const
{
	return ra_;
}

void Computer::set_memory(std::size_t address, const Word& value)
{
	if (address >= memory_.size())
	{
		throw std::out_of_range{"Invalid memory address"};
	}
	memory_[address] = value;
}

const Word& Computer::memory(int address, size_t index) const
{
	if (index != 0)
	{
		address += index_value(index);
	}

	return memory(address);
}

const Word& Computer::memory(int address) const
{
	if ((address < 0) || (address >= static_cast<int>(memory_.size())))
	{
		throw std::out_of_range{"Invalid Memory address"};
	}

	return memory_[static_cast<std::size_t>(address)];
}

int Computer::index_value(size_t index) const
{
	if ((index == 0) || (index > rindexes_.size()))
	{
		throw std::out_of_range{"Invalid InderRegister"};
	}

	return rindexes_[index - 1].value();
}

void Computer::execute(const Command& command)
{
	static_assert(k_commands_count == (Byte::k_max_value + 1),
		"Command actions array should have all possible variations of command IDs");

	auto callback = k_command_actions[command.id().cast_to<std::size_t>()];
	if (!callback)
	{
		throw std::exception{"Not implemented"};
	}
	
	(*this.*callback)(command);
}

void Computer::on_nop(const Command& /*command*/)
{
}

void Computer::on_lda(const Command& command)
{
	const auto& word = memory(
		command.address().value(),
		command.address_index().cast_to<std::size_t>());
	const bool ignore_sign = !command.field().includes_sign();
	ra_.set_value(word.value(ignore_sign), command.field());
}
