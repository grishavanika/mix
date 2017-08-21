#include <mix/computer.h>
#include <mix/command_processor.h>
#include <mix/computer_listener.h>

#include "internal/helpers.hpp"

using namespace mix;

Computer::Computer(IComputerListener* listener /*= nullptr*/)
	: ra_{}
	, rx_{}
	, rindexes_()
	, rj_{}
	, rip_{}
	, comparison_state_{ComparisonIndicator::Less}
	, overflow_flag_{OverflowFlag::NoOverdlow}
	, memory_()
	, devices_{listener}
	, listener_{listener}
{
	setup_default_devices();
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
	internal::InvokeListener(listener_, &IComputerListener::on_jump, next);
}

void Computer::set_next_command(int address)
{
	// #TODO: validate `address` (in range [0; 4000))
	rip_.set_value(address);
	internal::InvokeListener(listener_, &IComputerListener::on_current_command_changed, address);
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
		throw InvalidMemoryAddressIndex{address};
	}

	memory_[static_cast<std::size_t>(address)] = value;
	internal::InvokeListener(listener_, &IComputerListener::on_memory_set, address);
}

const Word& Computer::memory(int address) const
{
	if ((address < 0) || (address >= static_cast<int>(memory_.size())))
	{
		throw InvalidMemoryAddressIndex{address};
	}

	return memory_[static_cast<std::size_t>(address)];
}

const IndexRegister& Computer::ri(std::size_t index) const
{
	if ((index == 0) || (index > rindexes_.size()))
	{
		throw InvalidIndexRegister{index};
	}
	
	return rindexes_[index - 1];
}

void Computer::execute(const Command& command)
{
	// #TODO: make exception-safe on_before_command/on_after_command() calls ?
	internal::InvokeListener(listener_, &IComputerListener::on_before_command, command);

	CommandProcessor processor{*this};
	processor.process(command);

	internal::InvokeListener(listener_, &IComputerListener::on_after_command, command);
}

void Computer::set_ra(const Register& ra)
{
	ra_ = ra;
	internal::InvokeListener(listener_, &IComputerListener::on_ra_set);
}

void Computer::set_rx(const Register& rx)
{
	rx_ = rx;
	internal::InvokeListener(listener_, &IComputerListener::on_rx_set);
}

void Computer::set_ri(std::size_t index, const IndexRegister& ri)
{
	if ((index == 0) || (index > rindexes_.size()))
	{
		throw InvalidIndexRegister{index};
	}

	rindexes_[index - 1] = ri;
	internal::InvokeListener(listener_, &IComputerListener::on_ri_set, index);
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
	internal::InvokeListener(listener_, &IComputerListener::on_overflow_flag_set);
}

void Computer::clear_overflow()
{
	overflow_flag_ = OverflowFlag::NoOverdlow;
	internal::InvokeListener(listener_, &IComputerListener::on_overflow_flag_set);
}

void Computer::set_listener(IComputerListener* listener)
{
	listener_ = listener;
	devices_.set_listener(listener);
}

ComparisonIndicator Computer::comparison_state() const
{
	return comparison_state_;
}

void Computer::set_comparison_state(ComparisonIndicator comparison)
{
	comparison_state_ = comparison;
	internal::InvokeListener(listener_, &IComputerListener::on_comparison_state_set);
}

void Computer::halt()
{

}

IIODevice& Computer::device(DeviceId id)
{
	return devices_.device(id);
}

void Computer::replace_device(DeviceId id, std::unique_ptr<IIODevice> device)
{
	devices_.inject_device(id, std::move(device));
}

IIODevice& Computer::wait_device_ready(DeviceId id)
{
	auto& handle = device(id);
	while (!handle.ready())
	{
		internal::InvokeListener(listener_, &IComputerListener::on_wait_on_device, id);
	}
	return handle;
}

void Computer::setup_default_devices()
{
	// #TODO: add default devices to `devices_`
}
