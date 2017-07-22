#pragma once
#include <mix/command.h>
#include <mix/field.h>
#include <mix/word.h>

namespace mix {

Command make_lda(int address, const Field& field, std::size_t index_register);
Command make_lda(int address, const Field& field);
Command make_lda(int address, std::size_t index_register);
Command make_lda(int address);

} // namespace mix

