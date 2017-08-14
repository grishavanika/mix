#include <mix/device_controller.h>
#include <mix/io_device.h>

#include "internal/helpers.hpp"

#include <array>

#include <cassert>

using namespace mix;

namespace {

struct DeviceProxy final :
	public IIODevice
{
	DeviceProxy()
		: listener_{nullptr}
		, device_{}
		, id_{}
	{
	}

	DeviceProxy(IIODeviceListener& listener, DeviceId id, std::unique_ptr<IIODevice> device)
		: listener_{&listener}
		, device_{std::move(device)}
		, id_{id}
	{
	}

	virtual bool ready() const override
	{
		return device_->ready();
	}

	virtual int block_size() const override
	{
		return device_->block_size();
	}

	virtual Word read_next(DeviceBlockId block_id) override
	{
		listener_->on_device_read(id_, block_id);
		return device_->read_next(block_id);
	}

	virtual void write_next(DeviceBlockId block_id, const Word& word) override
	{
		listener_->on_device_write(id_, block_id);
		device_->write_next(block_id, word);
	}

private:
	IIODeviceListener* listener_;
	std::unique_ptr<IIODevice> device_;
	DeviceId id_;
};

} // namespace

struct DeviceController::Impl final :
	public IIODeviceListener
{
	IIODeviceListener* listener_;
	std::array<DeviceProxy, DeviceController::k_max_devices_count> devices_;

	Impl(IIODeviceListener* listener)
		: listener_{listener}
	{
	}

	IIODevice& device(DeviceId id)
	{
		return devices_.at(id);
	}

	void inject_device(DeviceId id, std::unique_ptr<IIODevice> device)
	{
		devices_.at(id) = DeviceProxy{*this, id, std::move(device)};
	}

	virtual void on_device_read(DeviceId id, DeviceBlockId block_id) override
	{
		internal::InvokeListener(listener_, &IIODeviceListener::on_device_read,
			id, block_id);
	}

	virtual void on_device_write(DeviceId id, DeviceBlockId block_id) override
	{
		internal::InvokeListener(listener_, &IIODeviceListener::on_device_write,
			id, block_id);
	}
};

DeviceController::DeviceController(IIODeviceListener* listener /*= nullptr*/)
	: impl_{std::make_unique<Impl>(listener)}
{
}

DeviceController::~DeviceController() = default;

void DeviceController::set_listener(IIODeviceListener* listener)
{
	impl_->listener_ = listener;
}

IIODevice& DeviceController::device(DeviceId id)
{
	return impl_->device(id);
}

void DeviceController::inject_device(DeviceId id, std::unique_ptr<IIODevice> device)
{
	assert(device);
	impl_->inject_device(id, std::move(device));
}

/*static*/ DeviceType DeviceController::DeviceTypeFromId(DeviceId id)
{
	const auto to_int = [](DeviceType type)
	{
		return static_cast<int>(type);
	};


	const auto raw_id = static_cast<int>(id);
	if (raw_id <= to_int(DeviceType::MagneticTape))
	{
		return DeviceType::MagneticTape;
	}
	else if (raw_id <= to_int(DeviceType::Drum))
	{
		return DeviceType::Drum;
	}
	else if (raw_id <= to_int(DeviceType::PunchCard))
	{
		return DeviceType::PunchCard;
	}
	else if (raw_id <= to_int(DeviceType::Perforator))
	{
		return DeviceType::Perforator;
	}
	else if (raw_id <= to_int(DeviceType::Printer))
	{
		return DeviceType::Terminal;
	}
	else if (raw_id <= to_int(DeviceType::PunchedTape))
	{
		return DeviceType::PunchedTape;
	}
	return DeviceType::Unknown;
}
