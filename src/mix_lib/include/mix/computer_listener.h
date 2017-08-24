#pragma once
#include <mix/config.h>
#include <mix/io_device.h>

#include <cstddef>

namespace mix {

class Command;

class MIX_LIB_EXPORT IComputerListener :
	public IIODeviceListener
{
public:
	virtual void on_memory_set(int /*address*/) {}
	virtual void on_ra_set() {}
	virtual void on_rx_set() {}
	virtual void on_ri_set(std::size_t /*index*/) {}
	virtual void on_overflow_flag_set() {}
	virtual void on_comparison_state_set() {}

	virtual void on_current_address_changed(int /*address*/) {};
	virtual void on_jump(int /*rj*/) {};

	virtual void on_before_command(const Command&) {};
	virtual void on_after_command(const Command&) {};

	virtual void on_wait_on_device(DeviceId) {}

protected:
	~IComputerListener() = default;
};

} // namespace mix

