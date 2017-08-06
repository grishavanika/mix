#include "precompiled.h"

using namespace mix;

using ::testing::_;
using ::testing::Return;
using ::testing::NiceMock;

TEST(IN, Reads_Device_Block_Size_Cells_To_Memory)
{
	auto device_mock = std::make_unique<DeviceMock>();
	EXPECT_CALL(*device_mock, ready()).WillOnce(Return(true));
	EXPECT_CALL(*device_mock, block_size()).WillOnce(Return(2));
	EXPECT_CALL(*device_mock, read_next(0)).WillRepeatedly(Return(Word{42}));
	EXPECT_CALL(*device_mock, write_next(_, _)).Times(0);

	ComputerListenerMock listener;
	EXPECT_CALL(listener, on_device_read(16, 0)).Times(2);
	EXPECT_CALL(listener, on_device_write(_, _)).Times(0);
	EXPECT_CALL(listener, on_wait_on_device(_)).Times(0);
	
	EXPECT_CALL(listener, on_memory_set(1000)).Times(1);
	EXPECT_CALL(listener, on_memory_set(1001)).Times(1);

	Computer mix{&listener};
	mix.replace_device(16, std::move(device_mock));

	mix.execute(MakeIN(1000, 16));

	ASSERT_EQ(Word{42}, mix.memory(1000));
	ASSERT_EQ(Word{42}, mix.memory(1001));
}

