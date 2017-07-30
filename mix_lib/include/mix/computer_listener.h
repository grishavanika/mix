#pragma once
#include <cstddef>

namespace mix {

struct IComputerListener
{
	virtual void on_memory_set(int /*address*/) {}
	virtual void on_ra_set() {}
	virtual void on_rx_set() {}
	virtual void on_ri_set(std::size_t /*index*/) {}
	virtual void on_overflow_flag_set() {}
	virtual void on_comparison_state_set() {}

	virtual void on_current_command_changed(int /*address*/) {};
	virtual void on_jump(int /*rj*/) {};

protected:
	~IComputerListener() = default;
};

} // namespace mix

