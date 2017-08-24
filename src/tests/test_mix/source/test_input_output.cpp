#include "precompiled.h"

using namespace mix;

using ::testing::_;
using ::testing::Return;
using ::testing::NiceMock;

template<typename... T>
DeviceMock::Block MakeBlock(T&&... word)
{
	DeviceMock::Block block;
	(void)std::initializer_list<int>{((void)block.push_back(Word{std::forward<T>(word)}), 0)...};
	return block;
}

int BlockSize(const DeviceMock::Block& block)
{
	return static_cast<int>(block.size());
}

TEST(IOInput, Reads_Device_Block_Size_Cells_To_Memory_From_Device)
{
	const DeviceId k_device_id = 16;

	auto device_mock = std::make_unique<DeviceMock>();
	const auto block = MakeBlock(42, 42);
	EXPECT_CALL(*device_mock, ready()).WillOnce(Return(true));
	EXPECT_CALL(*device_mock, read(0)).WillRepeatedly(Return(block));
	EXPECT_CALL(*device_mock, write_helper(_, _)).Times(0);

	NiceMock<ComputerListenerMock> listener;
	EXPECT_CALL(listener, on_device_read(k_device_id, 0)).Times(1);
	EXPECT_CALL(listener, on_device_write(_, _)).Times(0);
	EXPECT_CALL(listener, on_wait_on_device(_)).Times(0);
	
	EXPECT_CALL(listener, on_memory_set(1000)).Times(1);
	EXPECT_CALL(listener, on_memory_set(1001)).Times(1);

	Computer mix{&listener};
	mix.replace_device(k_device_id, std::move(device_mock));
	mix.set_rx(7); // Should be ignored

	mix.execute(MakeIN(1000, k_device_id));

	ASSERT_EQ(42, mix.memory(1000));
	ASSERT_EQ(42, mix.memory(1001));
}

TEST(IOInput, Waits_While_Device_Is_Busy)
{
	const DeviceId k_device_id = 16;

	auto device_mock = std::make_unique<DeviceMock>();
	const auto block = MakeBlock(42);
	EXPECT_CALL(*device_mock, ready())
		.WillOnce(Return(false))
		.WillOnce(Return(false))
		.WillOnce(Return(true));

	EXPECT_CALL(*device_mock, write_helper(_, _)).Times(0);

	NiceMock<ComputerListenerMock> listener;
	EXPECT_CALL(listener, on_device_read(k_device_id, 0)).Times(1);
	EXPECT_CALL(listener, on_wait_on_device(k_device_id)).Times(2);

	Computer mix{&listener};
	mix.replace_device(k_device_id, std::move(device_mock));
	mix.set_rx(Register{1}); // Should be ignored

	mix.execute(MakeIN(1000, k_device_id));
}

TEST(IOInput, Takes_Into_Account_Device_Block_ID_From_RX_For_Drum_Devices)
{
	const DeviceId k_device_id = 15;
	const DeviceBlockId k_drum_block_id = 4;

	ASSERT_EQ(DeviceType::Drum, DeviceController::DeviceTypeFromId(k_device_id));

	auto device_mock = std::make_unique<DeviceMock>();
	const auto block = MakeBlock(42);
	EXPECT_CALL(*device_mock, ready()).WillOnce(Return(true));
	EXPECT_CALL(*device_mock, read(k_drum_block_id)).WillRepeatedly(Return(block));
	EXPECT_CALL(*device_mock, write_helper(_, _)).Times(0);

	NiceMock<ComputerListenerMock> listener;
	EXPECT_CALL(listener, on_device_read(k_device_id, k_drum_block_id)).Times(1);

	Computer mix{&listener};
	mix.replace_device(k_device_id, std::move(device_mock));
	mix.set_rx(k_drum_block_id);

	mix.execute(MakeIN(1000, k_device_id));

	ASSERT_EQ(42, mix.memory(1000));
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
	mix.set_next_address(current_ip);

	mix.execute(MakeJRED(1000, k_device_id));

	ASSERT_EQ(1000, mix.next_address());
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
	mix.set_next_address(current_ip);

	mix.execute(MakeJBUS(1000, k_device_id));

	ASSERT_EQ(1000, mix.next_address());
}

TEST(IOOutput, Writes_Device_Block_Size_Cells_From_Memory_To_Device)
{
	const DeviceId k_device_id = 16;

	auto device_mock = std::make_unique<DeviceMock>();
	const auto block = MakeBlock(42, 42, 42);
	EXPECT_CALL(*device_mock, ready()).WillOnce(Return(true));
	EXPECT_CALL(*device_mock, block_size()).WillOnce(Return(BlockSize(block)));
	EXPECT_CALL(*device_mock, read(_)).Times(0);
	EXPECT_CALL(*device_mock, write_helper(0, block)).Times(1);

	NiceMock<ComputerListenerMock> listener;
	EXPECT_CALL(listener, on_device_read(_, _)).Times(0);
	EXPECT_CALL(listener, on_device_write(k_device_id, 0)).Times(1);
	EXPECT_CALL(listener, on_wait_on_device(_)).Times(0);

	Computer mix{&listener};
	mix.replace_device(k_device_id, std::move(device_mock));
	mix.set_rx(7); // Should be ignored

	mix.set_memory(1000, 42);
	mix.set_memory(1001, 42);
	mix.set_memory(1002, 42);

	mix.execute(MakeOUT(1000, k_device_id));
}

TEST(IOOutput, Takes_Into_Account_Device_Block_ID_From_RX_For_Drum_Devices)
{
	const DeviceId k_device_id = 12;
	const DeviceBlockId k_drum_block_id = 5;

	auto device_mock = std::make_unique<DeviceMock>();
	const auto block = MakeBlock(42);
	EXPECT_CALL(*device_mock, ready()).WillOnce(Return(true));
	EXPECT_CALL(*device_mock, block_size()).WillOnce(Return(BlockSize(block)));
	EXPECT_CALL(*device_mock, read(_)).Times(0);
	EXPECT_CALL(*device_mock, write_helper(k_drum_block_id, block)).Times(1);

	NiceMock<ComputerListenerMock> listener;
	EXPECT_CALL(listener, on_device_read(_, _)).Times(0);
	EXPECT_CALL(listener, on_device_write(k_device_id, k_drum_block_id)).Times(1);
	EXPECT_CALL(listener, on_wait_on_device(_)).Times(0);

	Computer mix{&listener};
	mix.replace_device(k_device_id, std::move(device_mock));
	mix.set_rx(Register{k_drum_block_id});
	mix.set_memory(1000, Word{42});

	mix.execute(MakeOUT(1000, k_device_id));
}

TEST(IOOutput, Waits_While_Device_Is_Busy)
{
	const DeviceId k_device_id = 12;
	const DeviceBlockId k_drum_block_id = 5;

	auto device_mock = std::make_unique<DeviceMock>();
	const auto block = MakeBlock(42);
	EXPECT_CALL(*device_mock, ready())
		.WillOnce(Return(false))
		.WillOnce(Return(false))
		.WillOnce(Return(false))
		.WillOnce(Return(false))
		.WillOnce(Return(true));
	EXPECT_CALL(*device_mock, block_size()).WillOnce(Return(BlockSize(block)));
	EXPECT_CALL(*device_mock, read(_)).Times(0);
	EXPECT_CALL(*device_mock, write_helper(k_drum_block_id, block)).Times(1);

	NiceMock<ComputerListenerMock> listener;
	EXPECT_CALL(listener, on_device_read(_, _)).Times(0);
	EXPECT_CALL(listener, on_device_write(k_device_id, k_drum_block_id)).Times(1);
	EXPECT_CALL(listener, on_wait_on_device(k_device_id)).Times(4);

	Computer mix{&listener};
	mix.replace_device(k_device_id, std::move(device_mock));
	mix.set_rx(k_drum_block_id);
	mix.set_memory(1000, 42);

	mix.execute(MakeOUT(1000, k_device_id));
}

