#pragma once
#include <mix/word.h>
#include <mix/general_types.h>

namespace mix {

class IIODeviceListener
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
	virtual ~IIODevice() = default;

	virtual bool ready() const = 0;
	virtual std::size_t block_size() const = 0;

	virtual Word read_next(DeviceBlockId block_id) = 0;
	virtual void write_next(DeviceBlockId block_id, const Word&) = 0;
};

} // namespace mix



