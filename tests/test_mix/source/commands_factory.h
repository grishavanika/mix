#pragma once
#include <mix/command.h>

namespace mix {

inline Command MakeADD(int address, const WordField& field = Word::MaxField(), std::size_t index_register = 0)
{
	return Command{1, address, index_register, field};
}

inline Command MakeSUB(int address, const WordField& field = Word::MaxField(), std::size_t index_register = 0)
{
	return Command{2, address, index_register, field};
}

inline Command MakeMUL(int address, const WordField& field = Word::MaxField(), std::size_t index_register = 0)
{
	return Command{3, address, index_register, field};
}

inline Command MakeDIV(int address, const WordField& field = Word::MaxField(), std::size_t index_register = 0)
{
	return Command{4, address, index_register, field};
}

inline Command MakeNUM()
{
	return Command{5, 0, 0, WordField::FromByte(0)};
}

inline Command MakeCHAR()
{
	return Command{5, 0, 0, WordField::FromByte(1)};
}

inline Command MakeLDA(int address, const WordField& field = Word::MaxField(), std::size_t index_register = 0)
{
	return Command{8, address, index_register, field};
}

inline Command MakeSTA(int address, const WordField& field = Word::MaxField(), std::size_t index_register = 0)
{
	return Command{24, address, index_register, field};
}

inline Command MakeIN(WordValue address, DeviceId device_id, std::size_t index_register = 0)
{
	return Command{36, address, index_register, WordField::FromByte(device_id)};
}

inline Command MakeJMP(WordValue address, std::size_t index_register = 0)
{
	return Command{39, address, index_register, WordField::FromByte(0)};
}

inline Command MakeJSJ(WordValue address, std::size_t index_register = 0)
{
	return Command{39, address, index_register, WordField::FromByte(1)};
}

inline Command MakeJOV(WordValue address, std::size_t index_register = 0)
{
	return Command{39, address, index_register, WordField::FromByte(2)};
}

inline Command MakeJNOV(WordValue address, std::size_t index_register = 0)
{
	return Command{39, address, index_register, WordField::FromByte(3)};
}

inline Command MakeJL(WordValue address, std::size_t index_register = 0)
{
	return Command{39, address, index_register, WordField::FromByte(4)};
}

inline Command MakeJE(WordValue address, std::size_t index_register = 0)
{
	return Command{39, address, index_register, WordField::FromByte(5)};
}

inline Command MakeJG(WordValue address, std::size_t index_register = 0)
{
	return Command{39, address, index_register, WordField::FromByte(6)};
}

inline Command MakeJGE(WordValue address, std::size_t index_register = 0)
{
	return Command{39, address, index_register, WordField::FromByte(7)};
}

inline Command MakeJNE(WordValue address, std::size_t index_register = 0)
{
	return Command{39, address, index_register, WordField::FromByte(8)};
}

inline Command MakeJLE(WordValue address, std::size_t index_register = 0)
{
	return Command{39, address, index_register, WordField::FromByte(9)};
}

inline Command MakeENTA(WordValue address, std::size_t index_register = 0)
{
	return Command{48, address, index_register, WordField::FromByte(2)};
}

inline Command MakeENTI(std::size_t index, WordValue address, std::size_t index_register = 0)
{
	return Command{48 + index, address, index_register, WordField::FromByte(2)};
}

inline Command MakeENNI(std::size_t index, WordValue address, std::size_t index_register = 0)
{
	return Command{48 + index, address, index_register, WordField::FromByte(3)};
}

inline Command MakeINCA(WordValue address, std::size_t index_register = 0)
{
	return Command{48, address, index_register, WordField::FromByte(0)};
}

inline Command MakeINCI(std::size_t index, WordValue address, std::size_t index_register = 0)
{
	return Command{48 + index, address, index_register, WordField::FromByte(0)};
}

inline Command MakeINCX(WordValue address, std::size_t index_register = 0)
{
	return Command{55, address, index_register, WordField::FromByte(0)};
}

inline Command MakeCMPA(int address, WordField field = Word::MaxField(), std::size_t index_register = 0)
{
	return Command{56, address, index_register, field};
}

inline Command MakeCMPI(std::size_t index, int address, WordField field = Word::MaxField(), std::size_t index_register = 0)
{
	return Command{56 + index, address, index_register, field};
}

inline Command MakeCMPX(int address, WordField field = Word::MaxField(), std::size_t index_register = 0)
{
	return Command{63, address, index_register, field};
}

} // namespace mix

