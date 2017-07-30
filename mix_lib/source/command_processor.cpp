#include <mix/command_processor.h>
#include <mix/computer.h>
#include <mix/command.h>

#include <cassert>
#include <cstdint>

using namespace mix;

/*static*/ const std::array<
	CommandProcessor::CommandAction,
	CommandProcessor::k_commands_count>
CommandProcessor::k_command_actions = {{
	/*00*/&CommandProcessor::nop,
	/*01*/&CommandProcessor::add,
	/*02*/&CommandProcessor::sub,
	/*03*/&CommandProcessor::mul,
	/*04*/&CommandProcessor::div,
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
	/*35*/nullptr,
	/*36*/nullptr,
	/*37*/nullptr,
	/*38*/nullptr,
	/*39*/nullptr,
	/*40*/nullptr,
	/*41*/nullptr,
	/*42*/nullptr,
	/*43*/nullptr,
	/*44*/nullptr,
	/*45*/nullptr,
	/*46*/nullptr,
	/*47*/nullptr,
	/*48*/&CommandProcessor::enta_group,
	/*49*/&CommandProcessor::ent1_group,
	/*50*/&CommandProcessor::ent2_group,
	/*51*/&CommandProcessor::ent3_group,
	/*52*/&CommandProcessor::ent4_group,
	/*53*/&CommandProcessor::ent5_group,
	/*54*/&CommandProcessor::ent6_group,
	/*55*/&CommandProcessor::entx_group,
	/*56*/nullptr,
	/*57*/nullptr,
	/*58*/nullptr,
	/*59*/nullptr,
	/*60*/nullptr,
	/*61*/nullptr,
	/*62*/nullptr,
	/*63*/nullptr
	}};

namespace {

int SignValue(int v)
{
	return (v < 0) ? -1 : 1;
}

Sign ReverseSign(Sign sign)
{
	return ((sign == Sign::Negative) ? Sign::Positive : Sign::Negative);
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

	(this->*callback)(command);
}

const Word& CommandProcessor::memory(const Command& command) const
{
	return mix_.memory(indexed_address(command));
}

void CommandProcessor::nop(const Command& /*command*/)
{
}

Register CommandProcessor::load_register(
	Register prev_value,
	const Command& command,
	bool reverse_sorce_sign /*= false*/) const
{
	const auto& word = memory(command);
	const auto& source_field = command.word_field();
	const auto dest_field = source_field.shift_bytes_right();

	auto value = word.value(source_field);
	
	prev_value.set_zero_abs_value();
	prev_value.set_value(reverse_sorce_sign ? value.reverse_sign() : value, dest_field);
	return std::move(prev_value);
}

int CommandProcessor::indexed_address(int address, std::size_t ri) const
{
	if (ri != 0)
	{
		return address + mix_.ri(ri).value();
	}

	return address;
}

int CommandProcessor::indexed_address(const Command& command) const
{
	return indexed_address(command.address(), command.address_index());
}

Register CommandProcessor::enter(const Command& command) const
{
	const auto value = indexed_address(command);
	
	Register r;
	r.set_value(WordValue{value});
	if (value == 0)
	{
		r.set_sign(command.sign());
	}
	return r;
}

Register CommandProcessor::enter_negative(const Command& command) const
{
	const auto value = indexed_address(command);

	Register r;
	r.set_value(WordValue{value}.reverse_sign());
	if (value == 0)
	{
		r.set_sign(ReverseSign(command.sign()));
	}
	return r;
}

void CommandProcessor::store_register(const Register& r, const Command& command)
{
	auto address = indexed_address(command);
	const auto& source_field = command.word_field();

	auto word = mix_.memory(address);
	word.set_value(
		r.value(source_field.shift_bytes_right()),
		source_field,
		false/*do not overwrite sign*/);
	
	mix_.set_memory(address, std::move(word));
}

IndexRegister CommandProcessor::load_index_register(
	std::size_t index,
	const Command& command,
	bool reverse_sorce_sign /*= false*/) const
{
	IndexRegister ri{load_register(mix_.ri(index), command, reverse_sorce_sign)};
	ri.zero_unspecified_bytes();
	return ri;
}

void CommandProcessor::lda(const Command& command)
{
	mix_.set_ra(load_register(mix_.ra(), command));
}

void CommandProcessor::ldx(const Command& command)
{
	mix_.set_rx(load_register(mix_.rx(), command));
}

void CommandProcessor::ld1(const Command& command)
{
	mix_.set_ri(1, load_index_register(1, command));
}

void CommandProcessor::ld2(const Command& command)
{
	mix_.set_ri(2, load_index_register(2, command));
}

void CommandProcessor::ld3(const Command& command)
{
	mix_.set_ri(3, load_index_register(3, command));
}

void CommandProcessor::ld4(const Command& command)
{
	mix_.set_ri(4, load_index_register(4, command));
}

void CommandProcessor::ld5(const Command& command)
{
	mix_.set_ri(5, load_index_register(5, command));
}

void CommandProcessor::ld6(const Command& command)
{
	mix_.set_ri(6, load_index_register(6, command));
}

void CommandProcessor::ldan(const Command& command)
{
	mix_.set_ra(load_register(mix_.ra(), command, true/*reverse*/));
}

void CommandProcessor::ldxn(const Command& command)
{
	mix_.set_rx(load_register(mix_.rx(), command, true/*reverse*/));
}

void CommandProcessor::ld1n(const Command& command)
{
	mix_.set_ri(1, load_index_register(1, command, true/*reverse*/));
}

void CommandProcessor::ld2n(const Command& command)
{
	mix_.set_ri(2, load_index_register(2, command, true/*reverse*/));
}

void CommandProcessor::ld3n(const Command& command)
{
	mix_.set_ri(3, load_index_register(3, command, true/*reverse*/));
}

void CommandProcessor::ld4n(const Command& command)
{
	mix_.set_ri(4, load_index_register(4, command, true/*reverse*/));
}

void CommandProcessor::ld5n(const Command& command)
{
	mix_.set_ri(5, load_index_register(5, command, true/*reverse*/));
}

void CommandProcessor::ld6n(const Command& command)
{
	mix_.set_ri(6, load_index_register(6, command, true/*reverse*/));
}

void CommandProcessor::sta(const Command& command)
{
	store_register(mix_.ra(), command);
}

void CommandProcessor::stx(const Command& command)
{
	store_register(mix_.rx(), command);
}

void CommandProcessor::st1(const Command& command)
{
	store_register(mix_.ri(1), command);
}

void CommandProcessor::st2(const Command& command)
{
	store_register(mix_.ri(2), command);
}

void CommandProcessor::st3(const Command& command)
{
	store_register(mix_.ri(3), command);
}

void CommandProcessor::st4(const Command& command)
{
	store_register(mix_.ri(4), command);
}

void CommandProcessor::st5(const Command& command)
{
	store_register(mix_.ri(5), command);
}

void CommandProcessor::st6(const Command& command)
{
	store_register(mix_.ri(6), command);
}

void CommandProcessor::stz(const Command& command)
{
	auto address = indexed_address(command);
	auto word = mix_.memory(address);
	word.set_value(0, command.word_field());
	mix_.set_memory(address, std::move(word));
}

void CommandProcessor::stj(const Command& command)
{
	// #TODO(MIXAL): default WordField should be (0, 2) instead of (0, 5)
	store_register(mix_.rj(), command);
}

Register CommandProcessor::do_safe_add_without_overflow_check(int value, int prev_value) const
{
	const int result = value + prev_value;

	Register ra;
	if (result == 0)
	{
		// We should not touch sign value,
		// hence result is 0 with unchanged sign
		ra.set_sign(mix_.ra().sign());
	}
	else
	{
		ra.set_value(result);
	}

	return ra;
}

Register CommandProcessor::do_add(const WordValue& value) const
{
	auto ra = mix_.ra();
	const int prev_value = ra.value();
	const bool overflow_possible = (value.sign() == ra.sign());
	
	if (overflow_possible)
	{
		int overflow_result = 0;
		if (CalculateWordAddOverflow(value, prev_value, overflow_result))
		{
			mix_.set_overflow();
			ra.set_value(overflow_result);
			return ra;
		}
	}

	return do_safe_add_without_overflow_check(value, prev_value);
}

void CommandProcessor::add(const Command& command)
{
	const auto value = memory(command).value(command.word_field());
	mix_.set_ra(do_add(value));
}

void CommandProcessor::sub(const Command& command)
{
	const auto value = memory(command).value(command.word_field());
	mix_.set_ra(do_add(value.reverse_sign()));
}

void CommandProcessor::mul(const Command& command)
{
	static_assert((sizeof(std::uint64_t) * CHAR_BIT) >= (2 * Word::k_bits_count),
		"Native `uint64_t` can't hold the result of MUL command");

	const auto ra = mix_.ra().value();
	const auto value = memory(command).value(command.word_field());
	const Sign sign = ((ra.sign() == value.sign()) ? Sign::Positive : Sign::Negative);
	const auto abs_result = std::uint64_t{ra.abs_value()} * value.abs_value();
	
	// Store less significant part to RX
	const auto rx_part = (abs_result & Word::k_max_abs_value);
	// ... and most significant part to RA
	const auto ra_part = ((abs_result >> Word::k_bits_count) & Word::k_max_abs_value);

	mix_.set_ra(Register{WordValue{sign, static_cast<int>(ra_part)}});
	mix_.set_rx(Register{WordValue{sign, static_cast<int>(rx_part)}});
}

void CommandProcessor::div(const Command& command)
{
	const auto ra = mix_.ra().value();
	const auto value = memory(command).value(command.word_field());

	const auto abs_ra = ra.abs_value();
	const auto abs_value = value.abs_value();
	if ((abs_ra >= abs_value) || (abs_value == 0))
	{
		mix_.set_overflow();
		return;
	}

	const auto rx = mix_.rx().value();
	std::uint64_t rax = (std::uint64_t{abs_ra} << Word::k_bits_count);
	rax |= rx.abs_value();

	const auto new_a = rax / abs_value;
	const auto new_x = rax % abs_value;
	const Sign prev_sign = ra.sign();
	const Sign sign = ((prev_sign == value.sign()) ? Sign::Positive : Sign::Negative);

	mix_.set_ra(Register{WordValue{sign, static_cast<int>(new_a)}});
	mix_.set_rx(Register{WordValue{prev_sign, static_cast<int>(new_x)}});
}

void CommandProcessor::enta_group(const Command& command)
{
	switch (command.field())
	{
	case 0:
		// INCA
		break;
	case 1:
		// DECA
		break;
	case 2:
		mix_.set_ra(enter(command));
		return;
	case 3:
		mix_.set_ra(enter_negative(command));
		return;
	}

	throw std::logic_error{"ENTA* command has unknown field"};
}

void CommandProcessor::entx_group(const Command& command)
{
	switch (command.field())
	{
	case 0:
		// INCX
		break;
	case 1:
		// DECX
		break;
	case 2:
		mix_.set_rx(enter(command));
		return;
	case 3:
		mix_.set_rx(enter_negative(command));
		return;
	}

	throw std::logic_error{"ENTX* command has unknown field"};
}

void CommandProcessor::enti_group(std::size_t index, const Command& command)
{
	switch (command.field())
	{
	case 0:
		// INCI
		break;
	case 1:
		// DECI
		break;
	case 2:
		mix_.set_ri(index, IndexRegister{enter(command)});
		return;
	case 3:
		mix_.set_ri(index, IndexRegister{enter_negative(command)});
		return;
	}

	throw std::logic_error{"ENTI* command has unknown field"};
}

void CommandProcessor::ent1_group(const Command& command)
{
	enti_group(1, command);
}

void CommandProcessor::ent2_group(const Command& command)
{
	enti_group(2, command);
}

void CommandProcessor::ent3_group(const Command& command)
{
	enti_group(3, command);
}

void CommandProcessor::ent4_group(const Command& command)
{
	enti_group(4, command);
}

void CommandProcessor::ent5_group(const Command& command)
{
	enti_group(5, command);
}

void CommandProcessor::ent6_group(const Command& command)
{
	enti_group(6, command);
}
