#pragma once
#include <mix/word.h>
#include <mix/general_types.h>

namespace mix {

class IIODeviceListener
{
public:
	virtual void on_device_read(DeviceId) {}
	virtual void on_device_write(DeviceId) {}

protected:
	~IIODeviceListener() = default;
};

class IIODevice
{
public:
	virtual ~IIODevice() = default;

	virtual bool ready() const = 0;
	virtual Word read() = 0;
	virtual void write(const Word&) = 0;
};

} // namespace mix



