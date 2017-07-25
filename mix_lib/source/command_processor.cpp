#include <mix/command_processor.h>
#include <mix/computer.h>
#include <mix/command.h>

#include <cassert>

using namespace mix;

/*static*/ const std::array<
	CommandProcessor::CommandAction,
	CommandProcessor::k_commands_count>
CommandProcessor::k_command_actions = {{
	/*00*/&CommandProcessor::nop,
	/*01*/&CommandProcessor::add,
	/*02*/&CommandProcessor::sub,
	/*03*/nullptr,
	/*04*/nullptr,
	/*05*/nullptr,
	/*06*/nullptr,
	/*07*/nullptr,
	/*08*/&CommandProcessor::lda,
	/*09*/&CommandProcessor::ld1,
	/*10*/&CommandProcessor::ld2,
	/*11*/&CommandProcessor::ld3,
	/*12*/&CommandProcessor::ld4,
	/*13*/&CommandProcessor::ld5,
	/*14*/&CommandProcessor::ld6,
	/*15*/&CommandProcessor::ldx,
	/*16*/&CommandProcessor::ldan,
	/*17*/&CommandProcessor::ld1n,
	/*18*/&CommandProcessor::ld2n,
	/*19*/&CommandProcessor::ld3n,
	/*20*/&CommandProcessor::ld4n,
	/*21*/&CommandProcessor::ld5n,
	/*22*/&CommandProcessor::ld6n,
	/*23*/&CommandProcessor::ldxn,
	/*24*/&CommandProcessor::sta,
	/*25*/&CommandProcessor::st1,
	/*26*/&CommandProcessor::st2,
	/*27*/&CommandProcessor::st3,
	/*28*/&CommandProcessor::st4,
	/*29*/&CommandProcessor::st5,
	/*30*/&CommandProcessor::st6,
	/*31*/&CommandProcessor::stx,
	/*32*/&CommandProcessor::stj,
	/*33*/&CommandProcessor::stz,
	/*34*/nullptr,
}};

namespace {

int SignValue(int v)
{
	return (v < 0) ? -1 : 1;
}

bool CalculateWordAddOverflow(int lhs, int rhs, int& overflow_part)
{
	assert(SignValue(lhs) == SignValue(rhs));

	const auto abs_rhs = static_cast<std::size_t>(std::abs(rhs));
	const auto part_without_overflow = (Word::k_max_abs_value - static_cast<std::size_t>(std::abs(lhs)));

	const bool overflow = (part_without_overflow < abs_rhs);
	if (overflow)
	{
		overflow_part = static_cast<int>(abs_rhs - part_without_overflow - 1);
		overflow_part *= SignValue(lhs);
	}

	return overflow;
}
} // namespace

CommandProcessor::CommandProcessor(Computer& mix)
	: mix_{mix}
{
}

void CommandProcessor::process(const Command& command)
{
	static_assert(k_commands_count == (Byte::k_max_value + 1),
		"Command actions array should have all possible variations of command IDs");

	auto callback = k_command_actions[command.id()];
	if (!callback)
	{
		throw std::runtime_error{"Not implemented"};
	}

	(*this.*callback)(command);
}

Word& CommandProcessor::memory(const Command& command)
{
	return mix_.memory_with_index(command.address(), command.address_index());
}

void CommandProcessor::nop(const Command& /*command*/)
{
}

void CommandProcessor::load_register(Register& r, const Command& command)
{
	const auto& word = memory(command);
	const auto& source_field = command.word_field();
	const auto dest_field = source_field.shift_bytes_right();

	r.set_zero_abs_value();
	r.set_value(word.value(source_field), dest_field);
}

void CommandProcessor::load_register_reverse_sign(Register& r, const Command& command)
{
	const auto& word = memory(command);
	const auto& source_field = command.word_field();
	const auto dest_field = source_field.shift_bytes_right();

	r.set_zero_abs_value();
	r.set_value(word.value(source_field).reverse_sign(), dest_field);	
}

void CommandProcessor::store_register(Register& r, const Command& command)
{
	auto& word = memory(command);
	const auto& source_field = command.word_field();

	word.set_value(
		r.value(source_field.shift_bytes_right()),
		source_field,
		false/*do not overwrite sign*/);
}

void CommandProcessor::load_index_register(std::size_t index, const Command& command)
{
	load_register(mix_.index_register(index), command);
}

void CommandProcessor::load_index_register_reverse_sign(std::size_t index, const Command& command)
{
	load_register_reverse_sign(mix_.index_register(index), command);
}

void CommandProcessor::lda(const Command& command)
{
	load_register(mix_.ra_, command);
}

void CommandProcessor::ldx(const Command& command)
{
	load_register(mix_.rx_, command);
}

void CommandProcessor::ld1(const Command& command)
{
	load_index_register(1, command);
}

void CommandProcessor::ld2(const Command& command)
{
	load_index_register(2, command);
}

void CommandProcessor::ld3(const Command& command)
{
	load_index_register(3, command);
}

void CommandProcessor::ld4(const Command& command)
{
	load_index_register(4, command);
}

void CommandProcessor::ld5(const Command& command)
{
	load_index_register(5, command);
}

void CommandProcessor::ld6(const Command& command)
{
	load_index_register(6, command);
}

void CommandProcessor::ldan(const Command& command)
{
	load_register_reverse_sign(mix_.ra_, command);
}

void CommandProcessor::ldxn(const Command& command)
{
	load_register_reverse_sign(mix_.rx_, command);
}

void CommandProcessor::ld1n(const Command& command)
{
	load_index_register_reverse_sign(1, command);
}

void CommandProcessor::ld2n(const Command& command)
{
	load_index_register_reverse_sign(2, command);
}

void CommandProcessor::ld3n(const Command& command)
{
	load_index_register_reverse_sign(3, command);
}

void CommandProcessor::ld4n(const Command& command)
{
	load_index_register_reverse_sign(4, command);
}

void CommandProcessor::ld5n(const Command& command)
{
	load_index_register_reverse_sign(5, command);
}

void CommandProcessor::ld6n(const Command& command)
{
	load_index_register_reverse_sign(6, command);
}

void CommandProcessor::sta(const Command& command)
{
	store_register(mix_.ra_, command);
}

void CommandProcessor::stx(const Command& command)
{
	store_register(mix_.rx_, command);
}

void CommandProcessor::st1(const Command& command)
{
	store_register(mix_.index_register(1), command);
}

void CommandProcessor::st2(const Command& command)
{
	store_register(mix_.index_register(2), command);
}

void CommandProcessor::st3(const Command& command)
{
	store_register(mix_.index_register(3), command);
}

void CommandProcessor::st4(const Command& command)
{
	store_register(mix_.index_register(4), command);
}

void CommandProcessor::st5(const Command& command)
{
	store_register(mix_.index_register(5), command);
}

void CommandProcessor::st6(const Command& command)
{
	store_register(mix_.index_register(6), command);
}

void CommandProcessor::stz(const Command& command)
{
	auto& word = memory(command);
	word.set_value(0, command.word_field());
}

void CommandProcessor::stj(const Command& command)
{
	// #TODO: default WordField should be (0, 2) instead of (0, 5)
	store_register(mix_.rj_, command);
}

void CommandProcessor::do_safe_add_without_overflow_check(int value, int prev_value)
{
	const int result = value + prev_value;

	if (result == 0)
	{
		// We should not touch sign value when,
		// hence setting only absolute part without changing sign
		mix_.ra_.set_zero_abs_value();
	}
	else
	{
		mix_.ra_.set_value(result);
	}
}

void CommandProcessor::do_add(const WordValue& value)
{
	const int prev_value = mix_.ra_.value();
	const bool overflow_possible = (value.sign() == mix_.ra_.sign());
	
	if (overflow_possible)
	{
		int overflow_result = 0;
		if (CalculateWordAddOverflow(value, prev_value, overflow_result))
		{
			mix_.ra_.set_value(overflow_result);
			mix_.set_overflow();
			return;
		}
	}

	do_safe_add_without_overflow_check(value, prev_value);
}

void CommandProcessor::add(const Command& command)
{
	const auto value = memory(command).value(command.word_field());
	do_add(value);
}

void CommandProcessor::sub(const Command& command)
{
	const auto value = memory(command).value(command.word_field());
	do_add(value.reverse_sign());
}
