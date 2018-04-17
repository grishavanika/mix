#pragma once
#include <mixal/line_translator.h>

#include <mix/computer.h>

#include <cstdio>

namespace mixal {

MIXAL_LIB_EXPORT
TranslatedProgram ParseProgramFromMDKStream(std::istream& stream);

MIXAL_LIB_EXPORT
TranslatedProgram ParseProgramFromMDKFile(const std::string& file_path);

MIXAL_LIB_EXPORT
void LoadProgramFromMDKStream(mix::Computer& computer, std::istream& stream);

MIXAL_LIB_EXPORT
void LoadProgramFromMDKFile(mix::Computer& computer, const std::string& file_path);

} // namespace mixal

