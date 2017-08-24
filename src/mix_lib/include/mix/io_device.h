#pragma once
#include <mix/config.h>
#include <mix/word.h>
#include <mix/general_types.h>

#include <vector>

namespace mix {

class MIX_LIB_EXPORT IIODeviceListener
{
public:
	virtual void on_device_read(DeviceId, DeviceBlockId) {}
	virtual void on_device_write(DeviceId, DeviceBlockId) {}

protected:
	~IIODeviceListener() = default;
};

class IIODevice
{
public:
	using Block = std::vector<Word>;

	virtual ~IIODevice() = default;

	virtual bool ready() const = 0;
	virtual int block_size() const = 0;

	virtual Block prepare_block() const = 0;
	virtual Block read(DeviceBlockId block_id) = 0;
	virtual void write(DeviceBlockId block_id, Block&&) = 0;
};

} // namespace mix



