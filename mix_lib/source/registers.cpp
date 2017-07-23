#include <mix/registers.h>
#include <mix/word_field.h>

using namespace mix;

void IndexRegister::set_byte(std::size_t index, const Byte& byte)
{
	if (is_undefined_index(index))
	{
		return;
	}

	Register::set_byte(index, byte);
}

bool IndexRegister::is_undefined_index(std::size_t index) const
{
	switch (index)
	{
	case 0:
	case 1:
	case 2:
		return true;
	}
	return false;
}


IndexRegister::IndexRegister()
{
}

IndexRegister::IndexRegister(const Byte& b4, const Byte& b5)
	: IndexRegister{Sign::Positive, b4, b5}
{
}

IndexRegister::IndexRegister(Sign sign, const Byte& b4, const Byte& b5)
{
	set_sign(sign);
	set_byte(4, b4);
	set_byte(5, b5);
}

IndexRegister::IndexRegister(int two_bytes_with_sign)
{
	set_value(
		two_bytes_with_sign,
		WordField{4, 5},
		true/*overwrite sign*/);
}

int IndexRegister::value() const
{
	return Register::value(WordField{4, 5});
}
