#include <mixal/operation_id.h>

#include <core/utils.h>

#include <cstring>

namespace mixal {
namespace {

const char* k_operation_str_table[] =
{
	"NOP",
	"ADD",
	"FADD",
	"SUB",
	"FSUB",
	"MUL",
	"FMUL",
	"DIV",
	"FDIV",
	"NUM",
	"CHAR",
	"HLT",
	"SLA",
	"SRA",
	"SLAX",
	"SRAX",
	"SLC",
	"SRC",
	"MOVE",
	"LDA",
	"LD1",
	"LD2",
	"LD3",
	"LD4",
	"LD5",
	"LD6",
	"LDX",
	"LDAN",
	"LD1N",
	"LD2N",
	"LD3N",
	"LD4N",
	"LD5N",
	"LD6N",
	"LDXN",
	"STA",
	"ST1",
	"ST2",
	"ST3",
	"ST4",
	"ST5",
	"ST6",
	"STX",
	"STJ",
	"STZ",
	"JBUS",
	"IOC",
	"IN",
	"OUT",
	"JRED",
	"JMP",
	"JSJ",
	"JOV",
	"JNOV",
	"JL",
	"JE",
	"JG",
	"JGE",
	"JNE",
	"JLE",
	"JA",
	"J1",
	"J2",
	"J3",
	"J4",
	"J5",
	"J6",
	"JX",
	"INCA",
	"DECA",
	"ENTA",
	"ENNA",
	"INC1",
	"DEC1",
	"ENT1",
	"ENN1",
	"INC2",
	"DEC2",
	"ENT2",
	"ENN2",
	"INC3",
	"DEC3",
	"ENT3",
	"ENN3",
	"INC4",
	"DEC4",
	"ENT4",
	"ENN4",
	"INC5",
	"DEC5",
	"ENT5",
	"ENN5",
	"INC6",
	"DEC6",
	"ENT6",
	"ENN6",
	"INCX",
	"DECX",
	"ENTX",
	"ENNX",
	"CMPA",
	"FCMP",
	"CMP1",
	"CMP2",
	"CMP3",
	"CMP4",
	"CMP5",
	"CMP6",
	"CMPX",
	"CON",
	"EQU",
	"ORIG",
	"END",
	"ALF",
};

static_assert(core::ArraySize(k_operation_str_table) == static_cast<std::size_t>(OperationId::Count),
	"Something wrong with OperationId mapping");

} // namespace

std::string_view OperationIdToString(OperationId id)
{
	if ((id > OperationId::Unknown) && (id < OperationId::Count))
	{
		return k_operation_str_table[static_cast<std::size_t>(id)];
	}

	return nullptr;
}

OperationId OperationIdFromString(std::string_view str)
{
	for (std::size_t index = 0; index < core::ArraySize(k_operation_str_table); ++index)
	{
		if (str == k_operation_str_table[index])
		{
			return static_cast<OperationId>(index);
		}
	}

	return OperationId::Unknown;
}

} // namespace mixal
