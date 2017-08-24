#pragma once
#include <mix/io_device.h>

namespace mix {

class DefaultDevice :
	public IIODevice
{
public:
	DefaultDevice(int block_size);

	virtual bool ready() const override;
	virtual int block_size() const override;

	virtual Word read_next(DeviceBlockId block_id) override;
	virtual void write_next(DeviceBlockId block_id, const Word&) override;

private:
	const int block_size_;
};

} // namespace mix



