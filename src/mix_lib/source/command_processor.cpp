#include <mix/command_processor.h>
#include <mix/computer.h>
#include <mix/command.h>
#include <mix/io_device.h>
#include <mix/char_table.h>

#include <algorithm>

#include <cassert>
#include <cstdint>
#include <cmath>

using namespace mix;

/*static*/ const std::array<
	CommandProcessor::CommandAction,
	Byte::k_values_count>
CommandProcessor::k_command_actions = {{
	/*00*/&CommandProcessor::nop,
	/*01*/&CommandProcessor::add,
	/*02*/&CommandProcessor::sub,
	/*03*/&CommandProcessor::mul,
	/*04*/&CommandProcessor::div,
	/*05*/&CommandProcessor::convert_or_halt_group,
	/*06*/&CommandProcessor::shift_group,
	/*07*/&CommandProcessor::move,
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
	/*34*/&CommandProcessor::jbus,
	/*35*/&CommandProcessor::ioc,
	/*36*/&CommandProcessor::in,
	/*37*/&CommandProcessor::out,
	/*38*/&CommandProcessor::jred,
	/*39*/&CommandProcessor::jmp_flags_group,
	/*40*/&CommandProcessor::jmp_ra_group,
	/*41*/&CommandProcessor::jmp_ri1_group,
	/*42*/&CommandProcessor::jmp_ri2_group,
	/*43*/&CommandProcessor::jmp_ri3_group,
	/*44*/&CommandProcessor::jmp_ri4_group,
	/*45*/&CommandProcessor::jmp_ri5_group,
	/*46*/&CommandProcessor::jmp_ri6_group,
	/*47*/&CommandProcessor::jmp_rx_group,
	/*48*/&CommandProcessor::enta_group,
	/*49*/&CommandProcessor::ent1_group,
	/*50*/&CommandProcessor::ent2_group,
	/*51*/&CommandProcessor::ent3_group,
	/*52*/&CommandProcessor::ent4_group,
	/*53*/&CommandProcessor::ent5_group,
	/*54*/&CommandProcessor::ent6_group,
	/*55*/&CommandProcessor::entx_group,
	/*56*/&CommandProcessor::cmpa,
	/*57*/&CommandProcessor::cmp1,
	/*58*/&CommandProcessor::cmp2,
	/*59*/&CommandProcessor::cmp3,
	/*60*/&CommandProcessor::cmp4,
	/*61*/&CommandProcessor::cmp5,
	/*62*/&CommandProcessor::cmp6,
	/*63*/&CommandProcessor::cmpx
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
	const auto part_without_overflow =
		(Word::k_max_abs_value - static_cast<std::size_t>(std::abs(lhs)));

	const bool overflow = (part_without_overflow < abs_rhs);
	if (overflow)
	{
		overflow_part = static_cast<int>(abs_rhs - part_without_overflow - 1);
		overflow_part *= SignValue(lhs);
	}

	return overflow;
}

RAX::Type RAXToNumber(const Register& r1, const Register& r2)
{
	std::size_t pow = RAX::k_bytes_count;
	RAX::Type result = 0;

	auto result_from_byte = [&](const Register& r)
	{
		for (const auto& byte : r.bytes())
		{
			--pow;
			result += ByteToDecimalDigit(byte) * static_cast<std::size_t>(std::pow(10u, pow));
		}
	};

	result_from_byte(r1);
	result_from_byte(r2);
	assert(pow == 0);

	return result;
}

} // namespace

CommandProcessor::CommandProcessor(Computer& mix)
	: mix_{mix}
{
}

void CommandProcessor::process(const Command& command)
{
	auto callback = k_command_actions[command.id()];
	assert(callback && "Invalid/not implemented command");

	(this->*callback)(command);
}

const Word& CommandProcessor::memory(const Command& command) const
{
	return mix_.memory(indexed_address(command));
}

void CommandProcessor::set_rax(const RAX& rax)
{
	mix_.set_ra(rax.ra);
	mix_.set_rx(rax.rx);
}

void CommandProcessor::nop(const Command& /*command*/)
{
}

Register CommandProcessor::do_load(
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
	
#if (0) // #INV (Clang): why "redundant move in return statement" ?
	return std::move(prev_value);
#else
	return prev_value;
#endif
}

int CommandProcessor::indexed_address(int address, std::size_t index) const
{
	if (index != 0)
	{
		return address + mix_.ri(index).value();
	}

	return address;
}

int CommandProcessor::indexed_address(const Command& command) const
{
	return indexed_address(command.address(), command.address_index());
}

Register CommandProcessor::do_enter(WordValue value, const Command& command) const
{
	Register r;
	r.set_value(value);
	if (value == 0)
	{
		r.set_sign(command.sign());
	}
	return r;
}

Register CommandProcessor::do_enter_negative(WordValue value, const Command& command) const
{
	Register r;
	r.set_value(value.reverse_sign());
	if (value == 0)
	{
		r.set_sign(ReverseSign(command.sign()));
	}
	return r;
}

void CommandProcessor::do_store(const Register& r, const Command& command)
{
	auto address = indexed_address(command);
	const auto& source_field = command.word_field();

	auto word = mix_.memory(address);
	const bool take_value_sign = source_field.includes_sign();
	word.set_value(
		r.value(source_field.shift_bytes_right(), take_value_sign),
		source_field,
		false/*do not overwrite sign*/);
	
	mix_.set_memory(address, std::move(word));
}

void CommandProcessor::lda(const Command& command)
{
	mix_.set_ra(do_load(mix_.ra(), command));
}

void CommandProcessor::ldx(const Command& command)
{
	mix_.set_rx(do_load(mix_.rx(), command));
}

void CommandProcessor::ld1(const Command& command)
{
	mix_.set_ri(1, IndexRegister{do_load(mix_.ri(1), command)});
}

void CommandProcessor::ld2(const Command& command)
{
	mix_.set_ri(2, IndexRegister{do_load(mix_.ri(2), command)});
}

void CommandProcessor::ld3(const Command& command)
{
	mix_.set_ri(3, IndexRegister{do_load(mix_.ri(3), command)});
}

void CommandProcessor::ld4(const Command& command)
{
	mix_.set_ri(4, IndexRegister{do_load(mix_.ri(4), command)});
}

void CommandProcessor::ld5(const Command& command)
{
	mix_.set_ri(5, IndexRegister{do_load(mix_.ri(5), command)});
}

void CommandProcessor::ld6(const Command& command)
{
	mix_.set_ri(6, IndexRegister{do_load(mix_.ri(6), command)});
}

void CommandProcessor::ldan(const Command& command)
{
	mix_.set_ra(do_load(mix_.ra(), command, true/*reverse*/));
}

void CommandProcessor::ldxn(const Command& command)
{
	mix_.set_rx(do_load(mix_.rx(), command, true/*reverse*/));
}

void CommandProcessor::ld1n(const Command& command)
{
	mix_.set_ri(1, IndexRegister{do_load(mix_.ri(1), command, true/*reverse*/)});
}

void CommandProcessor::ld2n(const Command& command)
{
	mix_.set_ri(2, IndexRegister{do_load(mix_.ri(2), command, true/*reverse*/)});
}

void CommandProcessor::ld3n(const Command& command)
{
	mix_.set_ri(3, IndexRegister{do_load(mix_.ri(3), command, true/*reverse*/)});
}

void CommandProcessor::ld4n(const Command& command)
{
	mix_.set_ri(4, IndexRegister{do_load(mix_.ri(4), command, true/*reverse*/)});
}

void CommandProcessor::ld5n(const Command& command)
{
	mix_.set_ri(5, IndexRegister{do_load(mix_.ri(5), command, true/*reverse*/)});
}

void CommandProcessor::ld6n(const Command& command)
{
	mix_.set_ri(6, IndexRegister{do_load(mix_.ri(6), command, true/*reverse*/)});
}

void CommandProcessor::sta(const Command& command)
{
	do_store(mix_.ra(), command);
}

void CommandProcessor::stx(const Command& command)
{
	do_store(mix_.rx(), command);
}

void CommandProcessor::st1(const Command& command)
{
	do_store(mix_.ri(1), command);
}

void CommandProcessor::st2(const Command& command)
{
	do_store(mix_.ri(2), command);
}

void CommandProcessor::st3(const Command& command)
{
	do_store(mix_.ri(3), command);
}

void CommandProcessor::st4(const Command& command)
{
	do_store(mix_.ri(4), command);
}

void CommandProcessor::st5(const Command& command)
{
	do_store(mix_.ri(5), command);
}

void CommandProcessor::st6(const Command& command)
{
	do_store(mix_.ri(6), command);
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
	do_store(mix_.rj(), command);
}

Register CommandProcessor::do_safe_add_without_overflow_check(Sign original_sign, int value, int prev_value) const
{
	const int result = value + prev_value;

	Register r;
	if (result == 0)
	{
		// We should not touch sign value,
		// hence result is 0 with unchanged sign
		r.set_sign(original_sign);
	}
	else
	{
		r.set_value(result);
	}

	return r;
}

Register CommandProcessor::do_add(Register r, const WordValue& value)
{
	const int prev_value = r.value();
	const bool overflow_possible = (value.sign() == r.sign());
	
	if (overflow_possible)
	{
		int overflow_result = 0;
		if (CalculateWordAddOverflow(value, prev_value, overflow_result))
		{
			mix_.set_overflow_flag(OverflowFlag::Overflow);
			r.set_value(overflow_result);
#if (0) // #INV (Clang): why "redundant move in return statement" ?
			return std::move(r);
#else
			return r;
#endif
		}
	}

	return do_safe_add_without_overflow_check(r.sign(), value, prev_value);
}

void CommandProcessor::add(const Command& command)
{
	const auto value = memory(command).value(command.word_field());
	mix_.set_ra(do_add(mix_.ra(), value));
}

void CommandProcessor::sub(const Command& command)
{
	const auto value = memory(command).value(command.word_field());
	mix_.set_ra(do_add(mix_.ra(), value.reverse_sign()));
}

void CommandProcessor::mul(const Command& command)
{
	const auto ra = mix_.ra().value();
	const auto value = memory(command).value(command.word_field());
	const Sign sign = ((ra.sign() == value.sign()) ? Sign::Positive : Sign::Negative);
	const auto abs_result = RAX::Type{ra.abs_value()} * value.abs_value();
	
	// Store less significant part to RX
	const auto rx_part = (abs_result & Word::k_max_abs_value);
	// ... and most significant part to RA
	const auto ra_part = ((abs_result >> Word::k_bits_count) & Word::k_max_abs_value);

	set_rax({{WordValue{sign, static_cast<int>(ra_part)}},
		{WordValue{sign, static_cast<int>(rx_part)}}});
}

void CommandProcessor::div(const Command& command)
{
	const auto ra = mix_.ra().value();
	const auto value = memory(command).value(command.word_field());

	const auto abs_ra = ra.abs_value();
	const auto abs_value = value.abs_value();
	if ((abs_value == 0) || (abs_ra >= abs_value))
	{
        mix_.set_overflow_flag(OverflowFlag::Overflow);
		return;
	}

	const auto rx = mix_.rx().value();
	RAX::Type rax = (RAX::Type{abs_ra} << Word::k_bits_count);
	rax |= rx.abs_value();

	const auto new_a = rax / abs_value;
	const auto new_x = rax % abs_value;
	const Sign prev_sign = ra.sign();
	const Sign sign = ((prev_sign == value.sign()) ? Sign::Positive : Sign::Negative);

	set_rax({{WordValue{sign, static_cast<int>(new_a)}},
		{WordValue{prev_sign, static_cast<int>(new_x)}}});
}

void CommandProcessor::enta_group(const Command& command)
{
	const WordValue value = indexed_address(command);
	const auto field = command.field();

	Register ra;
	switch (field)
	{
	case 0: // INCA
		ra = do_add(mix_.ra(), value);
		break;
	case 1: // DECA
		ra = do_add(mix_.ra(), value.reverse_sign());
		break;
	case 2: // ENTA
		ra = do_enter(value, command);
		break;
	case 3: // ENNA
		ra = do_enter_negative(value, command);
		break;
	default:
		throw UnknownCommandField{field};
	}

	mix_.set_ra(std::move(ra));
}

void CommandProcessor::entx_group(const Command& command)
{
	const WordValue value = indexed_address(command);
	const auto field = command.field();

	Register rx;
	switch (field)
	{
	case 0: // INCX
		rx = do_add(mix_.rx(), value);
		break;
	case 1: // DECX
		rx = do_add(mix_.rx(), value.reverse_sign());
		break;
	case 2: // ENTX
		rx = do_enter(value, command);
		break;
	case 3: // ENNX
		rx = do_enter_negative(value, command);
		break;
	default:
		throw UnknownCommandField{field};
	}

	mix_.set_rx(std::move(rx));
}

void CommandProcessor::enti_group(std::size_t index, const Command& command)
{
	const WordValue value = indexed_address(command);
	const auto field = command.field();

	Register result;
	switch (field)
	{
	case 0: // INCI
		// Note: overflow is not possible since `do_add()` works with 5 bytes overflow
		// and `IndexRegister` contains only 2 bytes
		result = do_add(mix_.ri(index), value);
		break;
	case 1: // DECI
		result = do_add(mix_.ri(index), value.reverse_sign());
		break;
	case 2: // ENTI
		result = do_enter(value, command);
		break;
	case 3: // ENNI
		result = do_enter_negative(value, command);
		break;
	default:
		throw UnknownCommandField{field};
	}

	mix_.set_ri(index, IndexRegister{result});
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

ComparisonIndicator CommandProcessor::do_compare(const Register& r, const Command& command) const
{
	const auto field = command.word_field();
	const int rhs = memory(command).value(field);
	const int lhs = r.value(field);

	if (lhs > rhs)
	{
		return ComparisonIndicator::Greater;
	}
	else if (lhs < rhs)
	{
		return ComparisonIndicator::Less;
	}

	return ComparisonIndicator::Equal;	
}

void CommandProcessor::cmpa(const Command& command)
{
	mix_.set_comparison_state(do_compare(mix_.ra(), command));
}

void CommandProcessor::cmpx(const Command& command)
{
	mix_.set_comparison_state(do_compare(mix_.rx(), command));
}

void CommandProcessor::cmp1(const Command& command)
{
	mix_.set_comparison_state(do_compare(mix_.ri(1), command));
}

void CommandProcessor::cmp2(const Command& command)
{
	mix_.set_comparison_state(do_compare(mix_.ri(2), command));
}

void CommandProcessor::cmp3(const Command& command)
{
	mix_.set_comparison_state(do_compare(mix_.ri(3), command));
}

void CommandProcessor::cmp4(const Command& command)
{
	mix_.set_comparison_state(do_compare(mix_.ri(4), command));
}

void CommandProcessor::cmp5(const Command& command)
{
	mix_.set_comparison_state(do_compare(mix_.ri(5), command));
}

void CommandProcessor::cmp6(const Command& command)
{
	mix_.set_comparison_state(do_compare(mix_.ri(6), command));
}

void CommandProcessor::jmp_flags_group(const Command& command)
{
	const int next_address = indexed_address(command);
	const ComparisonIndicator comparison_flag = mix_.comparison_state();
	const bool has_overflow = (mix_.overflow_flag() == OverflowFlag::Overflow);
	const auto field = command.field();

	bool do_jump = false;
	switch (field)
	{
	case 0: // JMP
		do_jump = true;
		break;
	case 1: // JSJ
		mix_.set_next_address(next_address);
		break;
	case 2: // JOV
		if (has_overflow)
		{
            mix_.set_overflow_flag(OverflowFlag::NoOverflow);
			do_jump = true;
		}
		break;
	case 3: // JNOV
		do_jump = !has_overflow;
		break;
	case 4: // JL
		do_jump = (comparison_flag == ComparisonIndicator::Less);
		break;
	case 5: // JE
		do_jump = (comparison_flag == ComparisonIndicator::Equal);
		break;
	case 6: // JG
		do_jump = (comparison_flag == ComparisonIndicator::Greater);
		break;
	case 7: // JGE
		do_jump = ((comparison_flag == ComparisonIndicator::Greater) ||
			(comparison_flag == ComparisonIndicator::Equal));
		break;
	case 8: // JNE
		do_jump = (comparison_flag != ComparisonIndicator::Equal);
		break;
	case 9: // JLE
		do_jump = ((comparison_flag == ComparisonIndicator::Less) ||
			(comparison_flag == ComparisonIndicator::Equal));
		break;
	default:
		throw UnknownCommandField{field};
	}

	if (do_jump)
	{
		mix_.jump(next_address);
	}
}

void CommandProcessor::do_jump(const Register& r, const Command& command)
{
	const int value = r.value();
	const auto field = command.field();

	bool do_jump = false;
	switch (field)
	{
	case 0:
		do_jump = (value < 0);
		break;
	case 1:
		do_jump = (value == 0);
		break;
	case 2:
		do_jump = (value > 0);
		break;
	case 3:
		do_jump = (value >= 0);
		break;
	case 4:
		do_jump = (value != 0);
		break;
	case 5:
		do_jump = (value <= 0);
		break;
	default:
		throw UnknownCommandField{field};
	}

	if (do_jump)
	{
		const int next_address = indexed_address(command);
		mix_.jump(next_address);
	}
}

void CommandProcessor::jmp_ra_group(const Command& command)
{
	do_jump(mix_.ra(), command);
}

void CommandProcessor::jmp_rx_group(const Command& command)
{
	do_jump(mix_.rx(), command);
}

void CommandProcessor::jmp_ri1_group(const Command& command)
{
	do_jump(mix_.ri(1), command);
}

void CommandProcessor::jmp_ri2_group(const Command& command)
{
	do_jump(mix_.ri(2), command);
}

void CommandProcessor::jmp_ri3_group(const Command& command)
{
	do_jump(mix_.ri(3), command);
}

void CommandProcessor::jmp_ri4_group(const Command& command)
{
	do_jump(mix_.ri(4), command);
}

void CommandProcessor::jmp_ri5_group(const Command& command)
{
	do_jump(mix_.ri(5), command);
}

void CommandProcessor::jmp_ri6_group(const Command& command)
{
	do_jump(mix_.ri(6), command);
}

void CommandProcessor::shift_group(const Command& command)
{
	const int shift = indexed_address(command);
	assert(shift >= 0);
	const auto field = command.field();

	switch (field)
	{
	case 0: // SLA
		ra_shift(+shift);
		break;
	case 1: // SRA
		ra_shift(-shift);
		break;
	case 2: // SLAX
		rax_shift(+shift, false/*non-cyclic*/);
		break;
	case 3: // SRAX
		rax_shift(-shift, false/*non-cyclic*/);
		break;
	case 4: // SLC
		rax_shift(+shift, true/*cyclic*/);
		break;
	case 5: // SRC
		rax_shift(-shift, true/*cyclic*/);
		break;
	default:
		throw UnknownCommandField{field};
	}
}

void CommandProcessor::ra_shift(int shift)
{
	const unsigned n = static_cast<unsigned>(std::abs(shift));
	auto bytes = mix_.ra().bytes();
	auto do_shift = [n](auto b, auto e)
	{
		std::rotate(b, b + n, e);
		std::fill(e - n, e, Byte());
	};
	if (shift >= 0)
	{
		do_shift(std::begin(bytes), std::end(bytes));
	}
	else
	{
		do_shift(std::rbegin(bytes), std::rend(bytes));
	}

	mix_.set_ra(Register(std::move(bytes), mix_.ra().sign()));
}

void CommandProcessor::rax_shift(int shift, bool cyclic)
{
	using AllBytes = std::array<Byte, 2 * Register::k_bytes_count>;
	AllBytes bytes;
	{ // Merge RA and RX to single bytes array
		std::copy(std::begin(mix_.ra().bytes()), std::end(mix_.ra().bytes())
			, std::begin(bytes));
		std::copy(std::begin(mix_.rx().bytes()), std::end(mix_.rx().bytes())
			, std::begin(bytes) + Register::k_bytes_count);
	}

	const unsigned n = static_cast<unsigned>(std::abs(shift));

	auto do_shift = [n, cyclic](auto b, auto e)
	{
		std::rotate(b, b + n, e);
		if (!cyclic)
		{
			std::fill(e - n, e, Byte());
		}
	};

	if (shift >= 0)
	{
		do_shift(std::begin(bytes), std::end(bytes));
	}
	else
	{
		do_shift(std::rbegin(bytes), std::rend(bytes));
	}

	{ // Reconstruct RA from first half
		Register::BytesArray ra;
		std::copy(std::begin(bytes), std::begin(bytes) + Register::k_bytes_count
			, std::begin(ra));
		mix_.set_ra(Register(std::move(ra), mix_.ra().sign()));
	}
	{ // Reconstruct RX from second half
		Register::BytesArray rx;
		std::copy(std::begin(bytes) + Register::k_bytes_count, std::end(bytes)
			, std::begin(rx));
		mix_.set_rx(Register(std::move(rx), mix_.rx().sign()));
	}
}

void CommandProcessor::move(const Command& command)
{
	const int source_address = indexed_address(command);
	assert(source_address >= 0);
	const auto r1 = mix_.ri(1);
	const int dest_address = r1.value();
	assert(dest_address >= 0);
	const int count = static_cast<int>(command.field());
	for (int i = 0; i < count; ++i)
	{
		mix_.set_memory(dest_address + i, mix_.memory(source_address + i));
	}
	mix_.set_ri(1, IndexRegister{do_add(r1, count)});
}

void CommandProcessor::convert_or_halt_group(const Command& command)
{
	const auto field = command.field();

	switch (field)
	{
	case 0: // NUM
		mix_.set_ra(num());
		break;
	case 1: // CHAR
		char_impl();
		break;
	case 2: // HLT
		mix_.halt();
		break;
	default:
		throw UnknownCommandField{field};
	}
}

Register CommandProcessor::num() const
{
	auto result = RAXToNumber(mix_.ra(), mix_.rx());
	if (result > Word::k_max_abs_value)
	{
		result %= Word::k_max_abs_value;
        mix_.set_overflow_flag(OverflowFlag::Overflow);
	}

	return Register{WordValue{mix_.ra().sign(), static_cast<int>(result)}};
}

void CommandProcessor::char_impl()
{
	auto value = mix_.ra().abs_value();

	auto make_register = [&](Sign sign)
	{
		Register r;
		r.set_sign(sign);
		for (std::size_t i = Word::k_bytes_count; i >= 1; --i)
		{
			r.set_byte(i, DecimalDigitToByte(value % 10));
			value /= 10;
		}
		return r;
	};

	mix_.set_rx(make_register(mix_.rx().sign()));
	mix_.set_ra(make_register(mix_.ra().sign()));
}

void CommandProcessor::in(const Command& command)
{
	const auto device_id = static_cast<DeviceId>(command.field());
	auto& device = mix_.wait_device_ready(device_id);

	const auto block_id = device_block_id(device_id);
	const int dest_address = indexed_address(command);
	const auto block = device.read(block_id);

	for (std::size_t i = 0, cells_count = block.size(); i < cells_count; ++i)
	{
		mix_.set_memory(dest_address + static_cast<int>(i), block[i]);
	}
}

void CommandProcessor::out(const Command& command)
{
	const auto device_id = static_cast<DeviceId>(command.field());
	auto& device = mix_.wait_device_ready(device_id);

	const auto block_id = device_block_id(device_id);
	const int source_address = indexed_address(command);

	auto block = device.prepare_block();
	for (std::size_t i = 0, cells_count = block.size(); i < cells_count; ++i)
	{
		block[i] = mix_.memory(source_address + static_cast<int>(i));
	}

	device.write(block_id, std::move(block));
}

void CommandProcessor::ioc(const Command& command)
{
	const auto device_id = static_cast<DeviceId>(command.field());
	auto& device = mix_.wait_device_ready(device_id);
	const auto block_id = device_block_id(device_id);
	(void)device;
	(void)block_id;
}

void CommandProcessor::jred(const Command& command)
{
	const auto device_id = static_cast<DeviceId>(command.field());
	const bool ready = mix_.device(device_id).ready();
	if (ready)
	{
		const int next_address = indexed_address(command);
		mix_.jump(next_address);
	}
}

void CommandProcessor::jbus(const Command& command)
{
	const auto device_id = static_cast<DeviceId>(command.field());
	const bool busy = !mix_.device(device_id).ready();
	if (busy)
	{
		const int next_address = indexed_address(command);
		mix_.jump(next_address);
	}
}

DeviceBlockId CommandProcessor::device_block_id(DeviceId device_id) const
{
	if (DeviceController::DeviceTypeFromId(device_id) == DeviceType::Drum)
	{
		return static_cast<DeviceBlockId>(mix_.rx().value());
	}

	return 0;
}
