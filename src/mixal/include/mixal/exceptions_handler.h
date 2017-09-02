#pragma once
#include <mixal/exceptions.h>
#include <mix/exceptions.h>

#include <iostream>

template<typename Callable>
void HandleAnyException(Callable callable)
{
	try
	{
		callable();
	}
	catch (const mixal::MixalException& mixal_error)
	{
		std::cerr << "[MIXAL] Error: " << mixal_error.what() << "\n";
	}
	catch (const mix::MixException& mix_error)
	{
		std::cerr << "[MIX] Error: " << mix_error.what() << "\n";
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << "\n";
	}
	catch (...)
	{
		std::cerr << "Error: " << "UNKNOWN" << "\n";
	}
}


