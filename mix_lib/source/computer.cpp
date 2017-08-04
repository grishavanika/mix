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
	, rip_{}
	, comparison_state_{ComparisonIndicator::Less}
	, overflow_flag_{OverflowFlag::NoOverdlow}
	, memory_{}
	, listener_{listener}
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

void Computer::jump(int address)
{
	const auto next = next_command();
	set_next_command(address);
	rj_.set_value(next);
	InvokeListener(listener_, &IComputerListener::on_jump, next);
}

void Computer::set_next_command(int address)
{
	// #TODO: validate `address` (in range [0; 4000))
	rip_.set_value(address);
	InvokeListener(listener_, &IComputerListener::on_current_command_changed, address);
}

int Computer::current_command() const
{
	return rip_.value();
}

int Computer::next_command() const
{
	return current_command() + 1;
}

void Computer::set_memory(int address, const Word& value)
{
	if ((address < 0) || (static_cast<std::size_t>(address) >= memory_.size()))
	{
		throw std::out_of_range{"Invalid memory address"};
	}

	memory_[static_cast<std::size_t>(address)] = value;
	InvokeListener(listener_, &IComputerListener::on_memory_set, address);
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
	// #TODO: make exception-safe on_before_command/on_after_command() calls ?
	InvokeListener(listener_, &IComputerListener::on_before_command, command);

	CommandProcessor processor{*this};
	processor.process(command);

	InvokeListener(listener_, &IComputerListener::on_after_command, command);
}

void Computer::set_ra(const Register& ra)
{
	ra_ = ra;
	InvokeListener(listener_, &IComputerListener::on_ra_set);
}

void Computer::set_rx(const Register& rx)
{
	rx_ = rx;
	InvokeListener(listener_, &IComputerListener::on_rx_set);
}

void Computer::set_ri(std::size_t index, const IndexRegister& ri)
{
	if ((index == 0) || (index > rindexes_.size()))
	{
		throw std::out_of_range{"Invalid IndexRegister"};
	}

	rindexes_[index - 1] = ri;
	InvokeListener(listener_, &IComputerListener::on_ri_set, index);
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
	InvokeListener(listener_, &IComputerListener::on_overflow_flag_set);
}

void Computer::clear_overflow()
{
	overflow_flag_ = OverflowFlag::NoOverdlow;
	InvokeListener(listener_, &IComputerListener::on_overflow_flag_set);
}

void Computer::set_listener(IComputerListener* listener)
{
	listener_ = listener;
}

ComparisonIndicator Computer::comparison_state() const
{
	return comparison_state_;
}

void Computer::set_comparison_state(ComparisonIndicator comparison)
{
	comparison_state_ = comparison;
	InvokeListener(listener_, &IComputerListener::on_comparison_state_set);
}

void Computer::halt()
{

}

