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

const AddressRegister& Computer::rj() const
{
	return rj_;
}

void Computer::set_memory(int address, const Word& value)
{
	if ((address < 0) || (static_cast<std::size_t>(address) >= memory_.size()))
	{
		throw std::out_of_range{"Invalid memory address"};
	}
	memory_[static_cast<std::size_t>(address)] = value;
}

const Word& Computer::memory(int address) const
{
	if ((address < 0) || (address >= static_cast<int>(memory_.size())))
	{
		throw std::out_of_range{"Invalid Memory address"};
	}

	return memory_[static_cast<std::size_t>(address)];
}

const IndexRegister& Computer::ri(std::size_t index) const
{
	if ((index == 0) || (index > rindexes_.size()))
	{
		throw std::out_of_range{"Invalid IndexRegister"};
	}
	
	return rindexes_[index - 1];
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
	if ((index == 0) || (index > rindexes_.size()))
	{
		throw std::out_of_range{"Invalid IndexRegister"};
	}

	rindexes_[index - 1] = ri;
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

