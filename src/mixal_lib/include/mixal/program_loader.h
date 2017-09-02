#pragma once
#include <mixal/line_translator.h>

#include <mix/computer.h>

namespace mixal {

MIXAL_LIB_EXPORT
void LoadProgram(mix::Computer& computer, const TranslatedProgram& program);

} // namespace mixal

