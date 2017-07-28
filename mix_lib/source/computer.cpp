#include <mix/computer.h>
#include <mix/command_processor.h>

using namespace mix;

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

int Computer::fix_up_address(int address, std::size_t ri) const
{
	if (ri != 0)
	{
		address += index_register(ri).value();
	}
	return address;
}

const Word& Computer::memory_with_index(int address, std::size_t ri) const
{
	return memory_at(fix_up_address(address, ri));
}

Word& Computer::memory_with_index(int address, std::size_t ri)
{
	return const_cast<Word&>(
		static_cast<const Computer&>(*this).memory_with_index(address, ri));
}

const Word& Computer::memory_at(int address) const
{
	if ((address < 0) || (address >= static_cast<int>(memory_.size())))
	{
		throw std::out_of_range{"Invalid Memory address"};
	}

	return memory_[static_cast<std::size_t>(address)];
}

const IndexRegister& Computer::index_register(std::size_t index) const
{
	if ((index == 0) || (index > rindexes_.size()))
	{
		throw std::out_of_range{"Invalid InderRegister"};
	}
	
	return rindexes_[index - 1];
}

IndexRegister& Computer::index_register(std::size_t index)
{
	return const_cast<IndexRegister&>(
		static_cast<const Computer&>(*this).index_register(index));
}

void Computer::execute(const Command& command)
{
	CommandProcessor processor{*this};
	processor.process(command);
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

OverflowFlag Computer::overflow_flag() const
{
	return overflow_flag_;
}

bool Computer::has_overflow() const
{
	return (overflow_flag_ == OverflowFlag::Overflow);
}

void Computer::set_overflow()
{
	overflow_flag_ = OverflowFlag::Overflow;
}

