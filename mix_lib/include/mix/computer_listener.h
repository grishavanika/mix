#pragma once
#include <cstddef>

namespace mix {

class Command;

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

	virtual void on_before_command(const Command&) {};
	virtual void on_after_command(const Command&) {};

protected:
	~IComputerListener() = default;
};

} // namespace mix

