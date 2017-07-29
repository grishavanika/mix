#include <mix/computer.h>
#include <mix/command_processor.h>
#include <mix/computer_listener.h>

using namespace mix;

namespace {

template<typename CallbackMember, typename... Args>
void InvokeListener(
	IComputerListener* listener,
	CallbackMember callback,
	Args&&... args)
{
	if (listener)
	{
		(listener->*callback)(std::forward<Args>(args)...);
	}
}
} // namespace

Computer::Computer(IComputerListener* listener /*= nullptr*/)
	: ra_{}
	, rx_{}
	, rindexes_{}
	, rj_{}
	, comparison_state_{ComparisonIndicator::Less}
	, overflow_flag_{OverflowFlag::NoOverdlow}
	, memory_{}
	, m_listener{listener}
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
	InvokeListener(m_listener, &IComputerListener::on_memory_set, address);
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
	InvokeListener(m_listener, &IComputerListener::on_ra_set);
}

void Computer::set_rx(const Register& rx)
{
	rx_ = rx;
	InvokeListener(m_listener, &IComputerListener::on_rx_set);
}

void Computer::set_ri(std::size_t index, const IndexRegister& ri)
{
	if ((index == 0) || (index > rindexes_.size()))
	{
		throw std::out_of_range{"Invalid IndexRegister"};
	}

	rindexes_[index - 1] = ri;
	InvokeListener(m_listener, &IComputerListener::on_ri_set, index);
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
	InvokeListener(m_listener, &IComputerListener::on_overflow_flag_set);
}

void Computer::set_listener(IComputerListener* listener)
{
	m_listener = listener;
}

