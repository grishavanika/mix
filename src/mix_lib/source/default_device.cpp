#include <mix/default_device.h>
#include <mix/char_table.h>

#include <iostream>

using namespace mix;

namespace {

// Very secret and unsuported MIX character will be as hint
// that something went wrong
const char k_unknown_MIX_char = '@';

} // namespace

SymbolDevice::SymbolDevice(int block_size, std::ostream& out, std::istream& in,
	bool handle_new_line /*= false*/)
		: block_size_{block_size}
		, current_column_{0}
		, out_{out}
		, in_{in}
		, handle_new_line_{handle_new_line}
{
}

bool SymbolDevice::ready() const
{
	return true;
}

int SymbolDevice::block_size() const
{
	return block_size_;
}

SymbolDevice::Block SymbolDevice::prepare_block() const
{
	Block block;
	block.resize(
		static_cast<std::size_t>(block_size()),
		word_with_all_spaces());
	return block;
}

SymbolDevice::Block SymbolDevice::read(DeviceBlockId /*block_id*/)
{
	auto block = prepare_block();

	bool has_new_line = false;
	for (auto& word : block)
	{
		word = read_word(has_new_line);
		if (has_new_line && handle_new_line_)
		{
			break;
		}
	}
	return block;
}

void SymbolDevice::write(DeviceBlockId /*block_id*/, Block&& block)
{
	for (auto byte : block)
	{
		write_word(byte);
	}
}

void SymbolDevice::write_word(const Word& word)
{
	for (auto byte : word.bytes())
	{
		write_byte(byte);
	}
}

void SymbolDevice::write_byte(const Byte& byte)
{
	bool converted = false;
	char ch = ByteToChar(byte, &converted);
	if (!converted)
	{
		ch = k_unknown_MIX_char;
	}

	out_ << ch;
	++current_column_;

	if (is_end_of_line())
	{
		start_new_line();
	}
}

void SymbolDevice::start_new_line()
{
	out_ << std::endl;
	current_column_ = 0;
}

bool SymbolDevice::is_end_of_line() const
{
	return (current_column_ == (block_size() * Word::k_bytes_count));
}

Byte SymbolDevice::read_byte(bool& is_new_line)
{
	char ch = k_unknown_MIX_char;
	in_ >> ch;
	is_new_line = (ch == '\n');
	return CharToByte(ch);
}

Word SymbolDevice::read_word(bool& is_new_line)
{
	auto word = word_with_all_spaces();

	for (std::size_t i = 1; i < Word::k_bytes_count; ++i)
	{
		auto readed_byte = read_byte(is_new_line);
		if (is_new_line && handle_new_line_)
		{
			break;
		}
		word.set_byte(i, readed_byte);
	}
	return word;
}

Word SymbolDevice::word_with_all_spaces() const
{
	Word::BytesArray bytes;
	bytes.fill(CharToByte(' '));
	return Word(std::move(bytes));
}

BinaryDevice::BinaryDevice(int block_size, std::ostream& out, std::istream& in)
	: block_size_{block_size}
	, out_{out}
	, in_{in}
{
}

bool BinaryDevice::ready() const
{
	return true;
}

int BinaryDevice::block_size() const
{
	return block_size_;
}

BinaryDevice::Block BinaryDevice::prepare_block() const
{
	Block block;
	block.resize(static_cast<std::size_t>(block_size()));
	return block;
}

BinaryDevice::Block BinaryDevice::read(DeviceBlockId /*block_id*/)
{
	auto block = prepare_block();
	for (auto& word : block)
	{
		word = read_word();
	}
	return block;
}

void BinaryDevice::write(DeviceBlockId /*block_id*/, Block&& block)
{
	for (const auto& word : block)
	{
		write_word(word);
	}
}

void BinaryDevice::write_word(const Word& word)
{
	out_ << int{word.value()};
}

Word BinaryDevice::read_word()
{
	int value = 0;
	in_ >> value;
	return Word(value);
}

