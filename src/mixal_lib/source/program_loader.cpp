#include <mixal/program_loader.h>

namespace mixal {

void LoadProgram(mix::Computer& computer, const TranslatedProgram& program)
{
	for (const auto& word : program.commands)
	{
		computer.set_memory(word.original_address, word.value);
	}
	computer.set_next_address(program.start_address);
}

} // namespace mixal


