#pragma once
#include <mix/general_types.h>

#include <memory>

namespace mix {

class IIODevice;
class IIODeviceListener;

class DeviceController
{
public:
	DeviceController(IIODeviceListener* listener = nullptr);
	~DeviceController();

	void set_listener(IIODeviceListener* listener);

	IIODevice& device(DeviceId id);
	void inject_device(std::unique_ptr<IIODevice> device);

private:
	struct Impl;
	std::unique_ptr<Impl> impl_;
};

} // namespace mix

