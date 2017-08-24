#include <mix/default_device.h>
#include <mix/char_table.h>

#include <iostream>

using namespace mix;

DefaultDevice::DefaultDevice(int block_size)
	: block_size_{block_size}
{
}

bool DefaultDevice::ready() const
{
	return true;
}

int DefaultDevice::block_size() const
{
	return block_size_;
}

Word DefaultDevice::read_next(DeviceBlockId /*block_id*/)
{
	return {};
}

void DefaultDevice::write_next(DeviceBlockId /*block_id*/, const Word& word)
{
	for (auto byte : word.bytes())
	{
		bool converted = false;
		char ch = ByteToChar(byte, &converted);
		if (!converted)
		{
			ch = 'x';
		}
	
		std::cout << ch;
	}
}

