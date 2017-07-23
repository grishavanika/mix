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
	/*09*/&Computer::on_ld1,
	/*10*/&Computer::on_ld2,
	/*11*/&Computer::on_ld3,
	/*12*/&Computer::on_ld4,
	/*13*/&Computer::on_ld5,
	/*14*/&Computer::on_ld6,
	/*15*/&Computer::on_ldx,
	/*16*/nullptr,
	/*17*/nullptr,
	/*18*/nullptr,
	/*19*/nullptr,
	/*20*/nullptr,
	/*21*/nullptr,
	/*22*/nullptr,
	/*23*/nullptr,
	/*23*/&Computer::on_sta,
};

Computer::Computer()
{
}

const Register& Computer::ra() const
{
	return ra_;
}

const Register& Computer::rx() const
{
	return rx_;
}

const IndexRegister& Computer::ri(std::size_t index) const
{
	return index_register(index);
}

void Computer::set_memory(std::size_t address, const Word& value)
{
	if (address >= memory_.size())
	{
		throw std::out_of_range{"Invalid memory address"};
	}
	memory_[address] = value;
}

const Word& Computer::memory_with_index(int address, size_t index) const
{
	if (index != 0)
	{
		address += index_register(index).value();
	}

	return memory_at(address);
}

Word& Computer::memory_with_index(int address, size_t index)
{
	return const_cast<Word&>(
		static_cast<const Computer&>(*this).memory_with_index(address, index));
}

const Word& Computer::memory_at(int address) const
{
	if ((address < 0) || (address >= static_cast<int>(memory_.size())))
	{
		throw std::out_of_range{"Invalid Memory address"};
	}

	return memory_[static_cast<std::size_t>(address)];
}

const IndexRegister& Computer::index_register(size_t index) const
{
	if ((index == 0) || (index > rindexes_.size()))
	{
		throw std::out_of_range{"Invalid InderRegister"};
	}
	
	return rindexes_[index - 1];
}

IndexRegister& Computer::index_register(size_t index)
{
	return const_cast<IndexRegister&>(
		static_cast<const Computer&>(*this).index_register(index));
}

void Computer::execute(const Command& command)
{
	static_assert(k_commands_count == (Byte::k_max_value + 1),
		"Command actions array should have all possible variations of command IDs");

	auto callback = k_command_actions[command.id()];
	if (!callback)
	{
		throw std::exception{"Not implemented"};
	}
	
	(*this.*callback)(command);
}

void Computer::on_nop(const Command& /*command*/)
{
}

void Computer::load_register(Register& r, const Command& command)
{
	const auto& word = memory_with_index(command.address(), command.address_index());
	const auto& source_field = command.word_field();

	// #TODO: we need this kind of special handling since
	// word.value(Field{0, 0}) will return 0 even if word's
	// sign is negative since there is no such thing as
	// negative integer zero
	if (source_field.has_only_sign())
	{
		r.set_sign(word.sign());
		return;
	}

	r.set_value(
		word.value(source_field),
		source_field.shift_bytes_right());
}

void Computer::load_index_register(std::size_t index, const Command& command)
{
	load_register(index_register(index), command);
}

void Computer::on_lda(const Command& command)
{
	load_register(ra_, command);
}

void Computer::on_ldx(const Command& command)
{
	load_register(rx_, command);
}

void Computer::on_ld1(const Command& command)
{
	load_index_register(1, command);
}

void Computer::on_ld2(const Command& command)
{
	load_index_register(2, command);
}

void Computer::on_ld3(const Command& command)
{
	load_index_register(3, command);
}

void Computer::on_ld4(const Command& command)
{
	load_index_register(4, command);
}

void Computer::on_ld5(const Command& command)
{
	load_index_register(5, command);
}

void Computer::on_ld6(const Command& command)
{
	load_index_register(6, command);
}

void Computer::on_sta(const Command& command)
{
	auto& word = memory_with_index(command.address(), command.address_index());
	const auto& source_field = command.word_field();

	if (source_field.has_only_sign())
	{
		word.set_sign(ra_.sign());
		return;
	}

	word.set_value(
		ra_.value(source_field.shift_bytes_right()),
		source_field);
}

void Computer::set_ra(const Register& ra)
{
	ra_ = ra;
}

void Computer::set_rx(const Register& rx)
{
	rx_ = rx;
}

void Computer::set_ri(std::size_t index, const IndexRegister& ri)
{
	index_register(index) = ri;
}

const Word& Computer::memory(std::size_t address) const
{
	return memory_at(static_cast<int>(address));
}
