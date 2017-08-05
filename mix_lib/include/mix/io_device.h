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

enum class IODeviceType
{
	Unknown				= -1,
	// https://en.wikipedia.org/wiki/Magnetic_tape
	MagneticTape		= 7,
	// https://en.wikipedia.org/wiki/Drum_memory
	Drum				= 15,
	// https://en.wikipedia.org/wiki/Punched_card
	PunchCard			= 16,
	Perforator			= 17,
	Printer				= 18,
	Terminal			= 19,
	// https://en.wikipedia.org/wiki/Punched_tape
	PunchedTape			= 20,
};

class IIODevice
{
public:
	virtual ~IIODevice() = default;

	virtual IODeviceType type() const = 0;
	virtual DeviceId id() const = 0;

	virtual bool ready() const = 0;
	virtual int block_size() const = 0;

	virtual Word read_next(DeviceBlockId block_id) = 0;
	virtual void write_next(DeviceBlockId block_id, const Word&) = 0;
};

} // namespace mix



