#pragma once

namespace mixal {

enum class OperationId
{
	Unknown	= -1,

	NOP		= 0,
	// ...
	CMPX	= 63,

	NativeBegin		= NOP,
	NativeEnd		= CMPX,
	
	CON		= 128,
	EQU		= 129,
	ORIG	= 130,
	END		= 131,
	ALF		= 132,

	PseudoBegin		= CON,
	PseudoEnd		= ALF,	
};

} // namespace mixal


