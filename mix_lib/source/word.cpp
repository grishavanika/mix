#include <mix/word.h>

using namespace mix;

Word::Word()
	: bytes_{}
	, sign_{Sign::Positive}
{
}

Sign Word::sign() const
{
	return sign_;
}

void Word::set_sign(Sign sign)
{
	sign_ = sign;
}

const Byte& Word::byte(std::size_t index) const
{
	if ((index == 0) || (index > bytes_.size()))
	{
		throw std::out_of_range{"Invalid byte's index for Word"};
	}
	return bytes_[index - 1];
}

void Word::set_byte(std::size_t index, const Byte& byte)
{
	auto& self_byte = const_cast<Byte&>(static_cast<const Word&>(*this).byte(index));
	self_byte = byte;
}
