#pragma once
#include <mix/io_device.h>

namespace mix {

class SymbolDevice final:
	public IIODevice
{
public:
	SymbolDevice(int block_size, std::ostream& out, std::istream& in,
		bool handle_new_line = false);

	virtual bool ready() const override;
	virtual int block_size() const override;

	virtual Block prepare_block() const override;
	virtual Block read(DeviceBlockId block_id) override;
	virtual void write(DeviceBlockId block_id, Block&&) override;

private:
	void write_word(const Word& word);
	void write_byte(const Byte& byte);
	Byte read_byte(bool& is_new_line);
	Word read_word(bool& is_new_line);

	Word word_with_all_spaces() const;

	bool is_end_of_line() const;
	void start_new_line();

private:
	const int block_size_;
	int current_column_;
	std::ostream& out_;
	std::istream& in_;
	bool handle_new_line_;
};

} // namespace mix



