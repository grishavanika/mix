#include <mix/field.h>

using namespace mix;

Field::Field(std::size_t left, std::size_t right)
	: left_{left}
	, right_{right}
{
	if (right > left)
	{
		throw std::logic_error{
			"Field can be used only for adjacent indexes"};
	}

	// #TODO: we can validate `left` and `right` even more:
	// they should be in range so `to_byte()` does not throw
}

Byte Field::to_byte() const
{
	const auto value = 8 * left_ + right_;
	return Byte{value};
}

Field Field::FromByte(const Byte& byte)
{
	const auto value = byte.cast_to<std::size_t>();
	return Field{value / 8, value % 8};
}

bool Field::has_only_sign() const
{
	return (left_ == 0) && (right_ == 0);
}

bool Field::includes_sign() const
{
	return (left_ == 0) && (right_ != 0);
}

std::size_t Field::left() const
{
	return left_;
}

std::size_t Field::right() const
{
	return right_;
}

