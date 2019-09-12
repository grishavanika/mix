#include <mix/computer.h>
#include <mix/command.h>
#include <mix/command_processor.h>
#include <mix/computer_listener.h>

#include <mix/default_device.h>

#include "internal/helpers.hpp"

#include <iostream>

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
	, halted_{false}
	, was_jump_{false}
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
	const auto next = next_address();
	set_next_address(address);
	rj_.set_value(next);
	was_jump_ = true;
	internal::InvokeListener(listener_, &IComputerListener::on_jump, next);
}

void Computer::set_next_address(int address)
{
	// #TODO: validate `address` (in range [0; 4000))
	if (address != current_address())
	{
		rip_.set_value(address);
		internal::InvokeListener(listener_, &IComputerListener::on_current_address_changed, address);
	}
}

int Computer::current_address() const
{
	return rip_.value();
}

int Computer::next_address() const
{
	if (was_jump_)
	{
		return current_address();
	}

	return current_address() + 1;
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

int Computer::run(int commands_count /*= -1*/)
{
	int executed_commands_count = 0;
	for (; (executed_commands_count < commands_count) || (commands_count < 0);
		++executed_commands_count)
	{
		if (!run_one())
		{
			break;
		}
	}
	return executed_commands_count;
}

bool Computer::run_one()
{
	if (halted_)
	{
		return false;
	}
    
    // #XXX: kill try/catch
    try
    {
        execute(Command{memory(current_address())});
        set_next_address(next_address());
        // Be sure to jump only once
        clear_jump_flag();
    }
    catch (const std::exception&)
    {
        halt();
        return false;
    }

	return true;
}

void Computer::halt()
{
	halted_ = true;
}

bool Computer::is_halted() const
{
    return halted_;
}

void Computer::clear_jump_flag()
{
	was_jump_ = false;
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
	// MagneticTape [0; 7] and Drum [8; 15]
	for (DeviceId id = 0; id <= 15; ++id)
	{
		devices_.inject_device(id,
			std::make_unique<BinaryDevice>(
				100/*block size*/,
				std::cout,
				std::cin));
	}

	const struct
	{
		const DeviceId id;
		const int block_size;
		const bool fill_new_line_with_spaces;
	} k_symbol_devices[] = {
		{16, 16, false}, // PunchCard
		{17, 16, false}, // Perforator
		{18, 24, false}, // Printer
		{19, 14, true}, // Terminal
		{20, 14, false}, // PunchedTape
	};

	for (auto symbol_device : k_symbol_devices)
	{
		devices_.inject_device(symbol_device.id,
			std::make_unique<SymbolDevice>(
				symbol_device.block_size,
				std::cout,
				std::cin,
				symbol_device.fill_new_line_with_spaces));
	}
}
