#include <mixal/program_executor.h>

namespace mixal {

void LoadProgram(mix::Computer& computer, const TranslatedProgram& program)
{
	computer.set_next_command(program.start_address);
	for (const auto& word : program.commands)
	{
		computer.set_memory(word.original_address, word.value);
	}
}

} // namespace mixal


