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
	"J1N",
	"J1Z",
	"J1P",
	"J1NN",
	"J1NZ",
	"J1NP",
	"J2N",
	"J2Z",
	"J2P",
	"J2NN",
	"J2NZ",
	"J2NP",
	"J3N",
	"J3Z",
	"J3P",
	"J3NN",
	"J3NZ",
	"J3NP",
	"J4N",
	"J4Z",
	"J4P",
	"J4NN",
	"J4NZ",
	"J4NP",
	"J5N",
	"J5Z",
	"J5P",
	"J5NN",
	"J5NZ",
	"J5NP",
	"J6N",
	"J6Z",
	"J6P",
	"J6NN",
	"J6NZ",
	"J6NP",
	"JAN",
	"JAZ",
	"JAP",
	"JANN",
	"JANZ",
	"JANP",
	"JXN",
	"JXZ",
	"JXP",
	"JXNN",
	"JXNZ",
	"JXNP",
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
