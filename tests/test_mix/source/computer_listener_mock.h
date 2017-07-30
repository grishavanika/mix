#pragma once
#include <mix/computer_listener.h>

#include <gmock/gmock.h>

struct ComputerListenerMock :
	public mix::IComputerListener
{
	MOCK_METHOD1(on_memory_set, void (int));
	MOCK_METHOD0(on_ra_set, void ());
	MOCK_METHOD0(on_rx_set, void ());
	MOCK_METHOD1(on_ri_set, void (std::size_t));
	MOCK_METHOD0(on_overflow_flag_set, void ());
	MOCK_METHOD0(on_comparison_state_set, void ());
};

