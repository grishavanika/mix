#pragma once
#include <mixal/config.h>

#include <core/string.h>

namespace mixal {

enum class OperationId
{
	Unknown	= -1,

	NOP		= 0,
	ADD		= 1,
	FADD	= 2,
	SUB		= 3,
	FSUB	= 4,
	MUL		= 5,
	FMUL	= 6,
	DIV		= 7,
	FDIV	= 8,
	NUM		= 9,
	CHAR	= 10,
	HLT		= 11,
	SLA		= 12,
	SRA		= 13,
	SLAX	= 14,
	SRAX	= 15,
	SLC		= 16,
	SRC		= 17,
	MOVE	= 18,
	LDA		= 19,
	LD1		= 20,
	LD2		= 21,
	LD3		= 22,
	LD4		= 23,
	LD5		= 24,
	LD6		= 25,
	LDX		= 26,
	LDAN	= 27,
	LD1N	= 28,
	LD2N	= 29,
	LD3N	= 30,
	LD4N	= 31,
	LD5N	= 32,
	LD6N	= 33,
	LDXN	= 34,
	STA		= 35,
	ST1		= 36,
	ST2		= 37,
	ST3		= 38,
	ST4		= 39,
	ST5		= 40,
	ST6		= 41,
	STX		= 42,
	STJ		= 43,
	STZ		= 44,
	JBUS	= 45,
	IOC		= 46,
	IN		= 47,
	OUT		= 48,
	JRED	= 49,
	JMP		= 50,
	JSJ		= 51,
	JOV		= 52,
	JNOV	= 53,
	JL		= 54,
	JE		= 55,
	JG		= 56,
	JGE		= 57,
	JNE		= 58,
	JLE		= 58,
	JA		= 60,
	J1		= 61,
	J2		= 62,
	J3		= 63,
	J4		= 64,
	J5		= 65,
	J6		= 66,
	JX		= 67,
	INCA	= 68,
	DECA	= 69,
	ENTA	= 70,
	ENNA	= 71,
	INC1	= 72,
	DEC1	= 73,
	ENT1	= 74,
	ENN1	= 75,
	INC2	= 76,
	DEC2	= 77,
	ENT2	= 78,
	ENN2	= 79,
	INC3	= 80,
	DEC3	= 81,
	ENT3	= 82,
	ENN3	= 83,
	INC4	= 84,
	DEC4	= 85,
	ENT4	= 86,
	ENN4	= 87,
	INC5	= 88,
	DEC5	= 89,
	ENT5	= 90,
	ENN5	= 91,
	INC6	= 92,
	DEC6	= 93,
	ENT6	= 94,
	ENN6	= 95,
	INCX	= 96,
	DECX	= 97,
	ENTX	= 98,
	ENNX	= 99,
	CMPA	= 100,
	FCMP	= 101,
	CMP1	= 102,
	CMP2	= 103,
	CMP3	= 104,
	CMP4	= 105,
	CMP5	= 106,
	CMP6	= 107,
	CMPX	= 108,

	J1N		= 109,
	J1Z		= 110,
	J1P		= 111,
	J1NN	= 112,
	J1NZ	= 113,
	J1NP	= 114,

	J2N		= 115,
	J2Z		= 116,
	J2P		= 117,
	J2NN	= 118,
	J2NZ	= 119,
	J2NP	= 120,

	J3N		= 121,
	J3Z		= 122,
	J3P		= 123,
	J3NN	= 124,
	J3NZ	= 125,
	J3NP	= 126,

	J4N		= 127,
	J4Z		= 128,
	J4P		= 129,
	J4NN	= 130,
	J4NZ	= 131,
	J4NP	= 132,

	J5N		= 133,
	J5Z		= 134,
	J5P		= 135,
	J5NN	= 136,
	J5NZ	= 137,
	J5NP	= 138,

	J6N		= 139,
	J6Z		= 140,
	J6P		= 141,
	J6NN	= 142,
	J6NZ	= 143,
	J6NP	= 144,

	JAN		= 145,
	JAZ		= 146,
	JAP		= 147,
	JANN	= 148,
	JANZ	= 149,
	JANP	= 150,

	JXN		= 151,
	JXZ		= 152,
	JXP		= 153,
	JXNN	= 154,
	JXNZ	= 155,
	JXNP	= 156,

	FIX		= 157, // Don't know about these operations yet.
	FLOT	= 158, // Adding them to be able to parse.
	SLB		= 159, // #TODO: implement in VM
	SRB		= 160,
	JAE		= 161,
	JAO		= 162,
	JXE		= 163,
	JXO		= 164,

	MIXOpBegin		= NOP,
	MIXOpEnd		= JXO,
	
	CON		= 165,
	EQU		= 166,
	ORIG	= 167,
	END		= 168,
	ALF		= 169,

	MIXALOpBegin	= CON,
	MIXALOpEnd		= ALF,

	Count
};

MIXAL_PARSER_LIB_EXPORT
std::string_view OperationIdToString(OperationId id);

MIXAL_PARSER_LIB_EXPORT
OperationId OperationIdFromString(std::string_view str);

MIXAL_PARSER_LIB_EXPORT
bool IsMIXOperation(OperationId id);

MIXAL_PARSER_LIB_EXPORT
bool IsMIXALOperation(OperationId id);

} // namespace mixal


