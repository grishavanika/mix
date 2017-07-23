#pragma once
#include <mix/command.h>
#include <mix/word_field.h>
#include <mix/word.h>

namespace mix {

Command make_lda(int address, const WordField& field, std::size_t index_register);
Command make_lda(int address, const WordField& field);
Command make_lda(int address, std::size_t index_register);
Command make_lda(int address);

Command make_sta(int address, const WordField& field, std::size_t index_register);
Command make_sta(int address, const WordField& field);
Command make_sta(int address, std::size_t index_register);
Command make_sta(int address);


} // namespace mix

