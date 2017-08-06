#include "precompiled.h"

using namespace mix;

using ::testing::_;
using ::testing::Return;
using ::testing::NiceMock;

TEST(IOInput, Reads_Device_Block_Size_Cells_To_Memory)
{
	const DeviceId k_device_id = 16;

	auto device_mock = std::make_unique<DeviceMock>();
	EXPECT_CALL(*device_mock, ready()).WillOnce(Return(true));
	EXPECT_CALL(*device_mock, block_size()).WillOnce(Return(2));
	EXPECT_CALL(*device_mock, read_next(0)).WillRepeatedly(Return(Word{42}));
	EXPECT_CALL(*device_mock, write_next(_, _)).Times(0);

	NiceMock<ComputerListenerMock> listener;
	EXPECT_CALL(listener, on_device_read(k_device_id, 0)).Times(2);
	EXPECT_CALL(listener, on_device_write(_, _)).Times(0);
	EXPECT_CALL(listener, on_wait_on_device(_)).Times(0);
	
	EXPECT_CALL(listener, on_memory_set(1000)).Times(1);
	EXPECT_CALL(listener, on_memory_set(1001)).Times(1);

	Computer mix{&listener};
	mix.replace_device(k_device_id, std::move(device_mock));
	mix.set_rx(Register{7}); // Should be ignored

	mix.execute(MakeIN(1000, k_device_id));

	ASSERT_EQ(Word{42}, mix.memory(1000));
	ASSERT_EQ(Word{42}, mix.memory(1001));
}

TEST(IOInput, Gets_Device_Block_ID_From_RX_For_Drum_Devices)
{
	const DeviceId k_device_id = 15;
	const DeviceBlockId k_drum_block_id = 4;

	ASSERT_EQ(DeviceType::Drum, DeviceController::DeviceTypeFromId(k_device_id));

	auto device_mock = std::make_unique<DeviceMock>();
	EXPECT_CALL(*device_mock, ready()).WillOnce(Return(true));
	EXPECT_CALL(*device_mock, block_size()).WillOnce(Return(1));
	EXPECT_CALL(*device_mock, read_next(k_drum_block_id)).WillRepeatedly(Return(Word{42}));
	EXPECT_CALL(*device_mock, write_next(_, _)).Times(0);

	NiceMock<ComputerListenerMock> listener;
	EXPECT_CALL(listener, on_device_read(k_device_id, k_drum_block_id)).Times(1);

	Computer mix{&listener};
	mix.replace_device(k_device_id, std::move(device_mock));
	mix.set_rx(Register{k_drum_block_id});

	mix.execute(MakeIN(1000, k_device_id));

	ASSERT_EQ(Word{42}, mix.memory(1000));
}

TEST(IOJRED, Does_Nothing_If_Device_Is_Busy)
{
	const DeviceId k_device_id = 16;

	auto device_mock = std::make_unique<NiceMock<DeviceMock>>();
	EXPECT_CALL(*device_mock, ready()).WillOnce(Return(false));

	NiceMock<ComputerListenerMock> listener;
	EXPECT_CALL(listener, on_jump(_)).Times(0);

	Computer mix{&listener};
	mix.replace_device(k_device_id, std::move(device_mock));

	mix.execute(MakeJRED(1000, k_device_id));
}

TEST(IOJRED, Does_Jump_If_Device_Is_Ready)
{
	const DeviceId k_device_id = 16;
	const int current_ip = 10;

	auto device_mock = std::make_unique<NiceMock<DeviceMock>>();
	EXPECT_CALL(*device_mock, ready()).WillOnce(Return(true));

	NiceMock<ComputerListenerMock> listener;
	EXPECT_CALL(listener, on_jump(current_ip + 1)).Times(1);

	Computer mix{&listener};
	mix.replace_device(k_device_id, std::move(device_mock));
	mix.set_next_command(current_ip);

	mix.execute(MakeJRED(1000, k_device_id));

	ASSERT_EQ(1001, mix.next_command());
}

TEST(IOJBUS, Does_Nothing_If_Device_Is_Ready)
{
	const DeviceId k_device_id = 16;

	auto device_mock = std::make_unique<NiceMock<DeviceMock>>();
	EXPECT_CALL(*device_mock, ready()).WillOnce(Return(true));

	NiceMock<ComputerListenerMock> listener;
	EXPECT_CALL(listener, on_jump(_)).Times(0);

	Computer mix{&listener};
	mix.replace_device(k_device_id, std::move(device_mock));

	mix.execute(MakeJBUS(1000, k_device_id));
}

TEST(IOJBUS, Does_Jump_If_Device_Is_Busy)
{
	const DeviceId k_device_id = 16;
	const int current_ip = 10;

	auto device_mock = std::make_unique<NiceMock<DeviceMock>>();
	EXPECT_CALL(*device_mock, ready()).WillOnce(Return(false));

	NiceMock<ComputerListenerMock> listener;
	EXPECT_CALL(listener, on_jump(current_ip + 1)).Times(1);

	Computer mix{&listener};
	mix.replace_device(k_device_id, std::move(device_mock));
	mix.set_next_command(current_ip);

	mix.execute(MakeJBUS(1000, k_device_id));

	ASSERT_EQ(1001, mix.next_command());
}

TEST(IOOutput, Reads_Device_Block_Size_Cells_To_Memory)
{
	const DeviceId k_device_id = 16;

	auto device_mock = std::make_unique<DeviceMock>();
	EXPECT_CALL(*device_mock, ready()).WillOnce(Return(true));
	EXPECT_CALL(*device_mock, block_size()).WillOnce(Return(3));
	EXPECT_CALL(*device_mock, read_next(_)).Times(0);
	EXPECT_CALL(*device_mock, write_next(0, Word{42})).Times(3);

	NiceMock<ComputerListenerMock> listener;
	EXPECT_CALL(listener, on_device_read(_, _)).Times(0);
	EXPECT_CALL(listener, on_device_write(k_device_id, 0)).Times(3);
	EXPECT_CALL(listener, on_wait_on_device(_)).Times(0);

	Computer mix{&listener};
	mix.replace_device(k_device_id, std::move(device_mock));
	mix.set_rx(Register{7}); // Should be ignored

	mix.set_memory(1000, Word{42});
	mix.set_memory(1001, Word{42});
	mix.set_memory(1002, Word{42});

	mix.execute(MakeOUT(1000, k_device_id));
}

