#include <mixal/mdk_program_loader.h>
#include <mixal/program_loader.h>
#include <mixal/exceptions.h>

#include <core/optional.h>

#include <fstream>

namespace mixal {

namespace {

// This part of code is piece of MDK that loads compiled file.
// Code can be found there: https://www.gnu.org/software/mdk/

// (Mostly) from mix_code_file.c
struct mix_cfheader_t
{
	std::int32_t signature;
	std::int32_t mj_ver;
	std::int32_t mn_ver;
	std::int16_t start;
	std::uint64_t path_len;
};

using mix_word_t = std::uint32_t;
using mix_short_t = std::uint16_t;

const std::int32_t k_signature = 0xDEADBEEF;
const std::int32_t k_signature_debug = 0xBEEFDEAD;
const mix_word_t k_mdk_word_sign_bit = (1L << 30);
const mix_word_t k_mdk_addr_tag = (k_mdk_word_sign_bit << 1);
const mix_word_t k_mdk_word_zero = 0;
const mix_word_t k_mdk_short_max = ((1L << 12) - 1);

bool IsInstructionWord(mix_word_t tagged)
{
	return ((tagged & k_mdk_addr_tag) == 0);
}

bool IsAddressWord(mix_word_t tagged)
{
	return ((tagged & k_mdk_addr_tag) == k_mdk_addr_tag);
}

Word ExtractInstruction(mix_word_t tagged)
{
	const bool has_sign = ((tagged & k_mdk_word_sign_bit) == k_mdk_word_sign_bit);
	if (has_sign)
	{
		return WordValue(mix::Sign::Negative
			, static_cast<WordValue::Type>(tagged & ~k_mdk_word_sign_bit));
	}

	return static_cast<WordValue::Type>(tagged);
}

int ExtractAddress(mix_word_t tagged)
{
	return static_cast<int>(tagged & k_mdk_short_max);
}

bool IsDebugSignature(std::int32_t signature)
{
	return (signature == k_signature_debug);
}

bool IsReleaseSignature(std::int32_t signature)
{
	return (signature == k_signature);
}

bool IsValidSignature(std::int32_t signature)
{
	return IsDebugSignature(signature) || IsReleaseSignature(signature);
}

std::optional<TranslatedWord> ParseNextWord(std::istream& stream
	, bool is_debug, int& address_counter)
{
	mix_word_t next = 0;
	while (true)
	{
		if (!stream.read(reinterpret_cast<char*>(&next), sizeof(next)))
		{
			return std::nullopt;
		}

		if (IsAddressWord(next))
		{
			address_counter = ExtractAddress(next);
		}
		else if (IsInstructionWord(next))
		{
			if (is_debug)
			{
				mix_short_t line_number = 0;
				if (!stream.read(reinterpret_cast<char*>(&line_number), sizeof(line_number)))
				{
					return std::nullopt;
				}
				(void)line_number;
			}

			TranslatedWord translated;
			translated.original_address = address_counter++;
			translated.value = ExtractInstruction(next);
			return translated;
		}
	}

	assert(false && "Not reachable");
	return std::nullopt;
}

} // namespace

TranslatedProgram ParseProgramFromMDKStream(std::istream& stream)
{
	// 1. Header
	mix_cfheader_t header{};
	if (!stream.read(reinterpret_cast<char*>(&header), sizeof(header)))
	{
		throw CorruptedMDKStream("failed to read header");
	}
	if (!IsValidSignature(header.signature))
	{
		throw CorruptedMDKStream("invalid signature");
	}
	// 2. Translated file path
	std::string file_path(header.path_len + 1, '\0');
	if (!stream.read(file_path.data(), header.path_len))
	{
		throw CorruptedMDKStream("failed to read file path");
	}
	(void)file_path;

	const bool is_debug = IsDebugSignature(header.signature);
	// 3. Debug symbol names and values
	if (is_debug)
	{
		while (stream.get() == ',')
		{
			std::string symbol;
			long value = 0;
			std::getline(stream, symbol, '=');
			stream >> value;
			(void)symbol;
			(void)value;
		}
	}

	// 5. Finally, bytecode
	TranslatedProgram mdk_program;
	int address = 0;
	while (auto translated = ParseNextWord(stream, is_debug, address))
	{
		mdk_program.commands.push_back(std::move(*translated));
	}

	mdk_program.start_address = header.start;
	mdk_program.completed = true;
	return mdk_program;
}

TranslatedProgram ParseProgramFromMDKFile(const std::string& file_path)
{
	std::ifstream input(file_path, std::ios_base::binary);
	return ParseProgramFromMDKStream(input);
}

void LoadProgramFromMDKFile(mix::Computer& computer, const std::string& file_path)
{
	LoadProgram(computer, ParseProgramFromMDKFile(file_path));
}

void LoadProgramFromMDKStream(mix::Computer& computer, std::istream& stream)
{
	LoadProgram(computer, ParseProgramFromMDKStream(stream));
}

} // namespace mixal


