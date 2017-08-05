#pragma once
#include <mix/general_types.h>

#include <memory>

namespace mix {

class IIODevice;
class IIODeviceListener;

class DeviceController
{
public:
	static constexpr std::size_t k_max_devices_count = 21;

public:
	DeviceController(IIODeviceListener* listener = nullptr);
	~DeviceController();

	void set_listener(IIODeviceListener* listener);

	IIODevice& device(DeviceId id);
	void inject_device(DeviceId id, std::unique_ptr<IIODevice> device);

	static IODeviceType DeviceTypeFromId(DeviceId id);

private:
	struct Impl;
	std::unique_ptr<Impl> impl_;
};

} // namespace mix

