#include <mixal_parse/types/operation_id.h>

#include <core/utils.h>

#include <cstring>

namespace mixal_parse {
namespace {

const struct
{
    const OperationId id;
    const char* const name;
} k_operation_str_table[] =
{
    {OperationId::NOP, "NOP"},
    {OperationId::ADD, "ADD"},
    {OperationId::FADD, "FADD"},
    {OperationId::SUB, "SUB"},
    {OperationId::FSUB, "FSUB"},
    {OperationId::MUL, "MUL"},
    {OperationId::FMUL, "FMUL"},
    {OperationId::DIV, "DIV"},
    {OperationId::FDIV, "FDIV"},
    {OperationId::NUM, "NUM"},
    {OperationId::CHAR, "CHAR"},
    {OperationId::HLT, "HLT"},
    {OperationId::SLA, "SLA"},
    {OperationId::SRA, "SRA"},
    {OperationId::SLAX, "SLAX"},
    {OperationId::SRAX, "SRAX"},
    {OperationId::SLC, "SLC"},
    {OperationId::SRC, "SRC"},
    {OperationId::MOVE, "MOVE"},
    {OperationId::LDA, "LDA"},
    {OperationId::LD1, "LD1"},
    {OperationId::LD2, "LD2"},
    {OperationId::LD3, "LD3"},
    {OperationId::LD4, "LD4"},
    {OperationId::LD5, "LD5"},
    {OperationId::LD6, "LD6"},
    {OperationId::LDX, "LDX"},
    {OperationId::LDAN, "LDAN"},
    {OperationId::LD1N, "LD1N"},
    {OperationId::LD2N, "LD2N"},
    {OperationId::LD3N, "LD3N"},
    {OperationId::LD4N, "LD4N"},
    {OperationId::LD5N, "LD5N"},
    {OperationId::LD6N, "LD6N"},
    {OperationId::LDXN, "LDXN"},
    {OperationId::STA, "STA"},
    {OperationId::ST1, "ST1"},
    {OperationId::ST2, "ST2"},
    {OperationId::ST3, "ST3"},
    {OperationId::ST4, "ST4"},
    {OperationId::ST5, "ST5"},
    {OperationId::ST6, "ST6"},
    {OperationId::STX, "STX"},
    {OperationId::STJ, "STJ"},
    {OperationId::STZ, "STZ"},
    {OperationId::JBUS, "JBUS"},
    {OperationId::IOC, "IOC"},
    {OperationId::IN, "IN"},
    {OperationId::OUT, "OUT"},
    {OperationId::JRED, "JRED"},
    {OperationId::JMP, "JMP"},
    {OperationId::JSJ, "JSJ"},
    {OperationId::JOV, "JOV"},
    {OperationId::JNOV, "JNOV"},
    {OperationId::JL, "JL"},
    {OperationId::JE, "JE"},
    {OperationId::JG, "JG"},
    {OperationId::JGE, "JGE"},
    {OperationId::JNE, "JNE"},
    {OperationId::JLE, "JLE"},
    {OperationId::INCA, "INCA"},
    {OperationId::DECA, "DECA"},
    {OperationId::ENTA, "ENTA"},
    {OperationId::ENNA, "ENNA"},
    {OperationId::INC1, "INC1"},
    {OperationId::DEC1, "DEC1"},
    {OperationId::ENT1, "ENT1"},
    {OperationId::ENN1, "ENN1"},
    {OperationId::INC2, "INC2"},
    {OperationId::DEC2, "DEC2"},
    {OperationId::ENT2, "ENT2"},
    {OperationId::ENN2, "ENN2"},
    {OperationId::INC3, "INC3"},
    {OperationId::DEC3, "DEC3"},
    {OperationId::ENT3, "ENT3"},
    {OperationId::ENN3, "ENN3"},
    {OperationId::INC4, "INC4"},
    {OperationId::DEC4, "DEC4"},
    {OperationId::ENT4, "ENT4"},
    {OperationId::ENN4, "ENN4"},
    {OperationId::INC5, "INC5"},
    {OperationId::DEC5, "DEC5"},
    {OperationId::ENT5, "ENT5"},
    {OperationId::ENN5, "ENN5"},
    {OperationId::INC6, "INC6"},
    {OperationId::DEC6, "DEC6"},
    {OperationId::ENT6, "ENT6"},
    {OperationId::ENN6, "ENN6"},
    {OperationId::INCX, "INCX"},
    {OperationId::DECX, "DECX"},
    {OperationId::ENTX, "ENTX"},
    {OperationId::ENNX, "ENNX"},
    {OperationId::CMPA, "CMPA"},
    {OperationId::FCMP, "FCMP"},
    {OperationId::CMP1, "CMP1"},
    {OperationId::CMP2, "CMP2"},
    {OperationId::CMP3, "CMP3"},
    {OperationId::CMP4, "CMP4"},
    {OperationId::CMP5, "CMP5"},
    {OperationId::CMP6, "CMP6"},
    {OperationId::CMPX, "CMPX"},
    {OperationId::J1N, "J1N"},
    {OperationId::J1Z, "J1Z"},
    {OperationId::J1P, "J1P"},
    {OperationId::J1NN, "J1NN"},
    {OperationId::J1NZ, "J1NZ"},
    {OperationId::J1NP, "J1NP"},
    {OperationId::J2N, "J2N"},
    {OperationId::J2Z, "J2Z"},
    {OperationId::J2P, "J2P"},
    {OperationId::J2NN, "J2NN"},
    {OperationId::J2NZ, "J2NZ"},
    {OperationId::J2NP, "J2NP"},
    {OperationId::J3N, "J3N"},
    {OperationId::J3Z, "J3Z"},
    {OperationId::J3P, "J3P"},
    {OperationId::J3NN, "J3NN"},
    {OperationId::J3NZ, "J3NZ"},
    {OperationId::J3NP, "J3NP"},
    {OperationId::J4N, "J4N"},
    {OperationId::J4Z, "J4Z"},
    {OperationId::J4P, "J4P"},
    {OperationId::J4NN, "J4NN"},
    {OperationId::J4NZ, "J4NZ"},
    {OperationId::J4NP, "J4NP"},
    {OperationId::J5N, "J5N"},
    {OperationId::J5Z, "J5Z"},
    {OperationId::J5P, "J5P"},
    {OperationId::J5NN, "J5NN"},
    {OperationId::J5NZ, "J5NZ"},
    {OperationId::J5NP, "J5NP"},
    {OperationId::J6N, "J6N"},
    {OperationId::J6Z, "J6Z"},
    {OperationId::J6P, "J6P"},
    {OperationId::J6NN, "J6NN"},
    {OperationId::J6NZ, "J6NZ"},
    {OperationId::J6NP, "J6NP"},
    {OperationId::JAN, "JAN"},
    {OperationId::JAZ, "JAZ"},
    {OperationId::JAP, "JAP"},
    {OperationId::JANN, "JANN"},
    {OperationId::JANZ, "JANZ"},
    {OperationId::JANP, "JANP"},
    {OperationId::JXN, "JXN"},
    {OperationId::JXZ, "JXZ"},
    {OperationId::JXP, "JXP"},
    {OperationId::JXNN, "JXNN"},
    {OperationId::JXNZ, "JXNZ"},
    {OperationId::JXNP, "JXNP"},
    {OperationId::FIX, "FIX"},
    {OperationId::FLOT, "FLOT"},
    {OperationId::SLB, "SLB"},
    {OperationId::SRB, "SRB"},
    {OperationId::JAE, "JAE"},
    {OperationId::JAO, "JAO"},
    {OperationId::JXE, "JXE"},
    {OperationId::JXO, "JXO"},
    {OperationId::CON, "CON"},
    {OperationId::EQU, "EQU"},
    {OperationId::ORIG, "ORIG"},
    {OperationId::END, "END"},
    {OperationId::ALF, "ALF"},
};

static_assert(core::ArraySize(k_operation_str_table) == static_cast<std::size_t>(OperationId::Count),
	"Something wrong with OperationId mapping");

} // namespace

std::string_view OperationIdToString(OperationId id)
{
    if ((id == OperationId::Unknown)
        || (id == OperationId::Count))
    {
        return std::string_view();
    }

    for (const auto& info : k_operation_str_table)
    {
        if (info.id == id)
        {
            return info.name;
        }
    }
	return std::string_view();
}

OperationId OperationIdFromString(std::string_view str)
{
    for (const auto& info : k_operation_str_table)
    {
        if (str == info.name)
        {
            return info.id;
        }
    }
	return OperationId::Unknown;
}

bool IsMIXOperation(OperationId id)
{
	return (id >= OperationId::MIXOpBegin) &&
		(id <= OperationId::MIXOpEnd);
}

bool IsMIXALOperation(OperationId id)
{
	return (id >= OperationId::MIXALOpBegin) &&
		(id <= OperationId::MIXALOpEnd);
}

} // namespace mixal_parse
