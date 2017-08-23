#include <mixal/translator.h>
#include <mixal/exceptions.h>
#include <mixal/operations_calculator.h>
#include <mixal/operation_info.h>

#include <mix/char_table.h>
#include <mix/word_field.h>
#include <mix/command.h>

#include <functional>
#include <algorithm>
#include <list>

using namespace mixal;

namespace {

Expression MakeExression(const std::string_view& symbol)
{
	Expression::Token token;
	token.basic_expr = BasicExpression{symbol, BasicExpression::Kind::Symbol};
	Expression expr;
	expr.add_token(std::move(token));
	return expr;
}

struct AddressTransformation
{
	std::vector<Symbol> forward_references;
	Address address;
};

template<typename K, typename V>
FlatMap<K, V> CombineFlatMaps(FlatMap<K, V>&& fm1, FlatMap<K, V>&& fm2)
{
	FlatMap<K, V> result = std::move(fm1);
	result.reserve(fm1.size() + fm2.size());
	copy(make_move_iterator(fm2.begin()),
		make_move_iterator(fm2.end()),
		back_inserter(result));
	return result;
}

} // namespace

struct Translator::Impl
{
	Impl(const DefinedSymbols& symbols,
		const DefinedLocalSymbols& local_symbols,
		int current_address);

	Word evaluate(const Text& text) const;
	Word evaluate(const Symbol& symbol) const;
	Word evaluate(const Number& n) const;
	Word evaluate(const BasicExpression& expr) const;
	Word evaluate(const Expression& expr) const;
	Word evaluate(const WValue& wvalue) const;

	FutureTranslatedWordRef translate_MIX(
		Operation command,
		const Address& A, const Index& I, const Field& F,
		const Label& label);

	void translate_EQU(const WValue& value, const Label& label);
	void translate_ORIG(const WValue& address, const Label& label);

	// #TODO: `WValue` can contain `Forward reference`, hence we should return `FutureWord`
	TranslatedWord translate_CON(const WValue& address, const Label& label);
	TranslatedWord translate_CON(const Word& value, const Label& label);
	TranslatedWord translate_ALF(const Text& text, const Label& label);
	EndCommandGeneratedCode translate_END(const WValue& address, const Label& label);

	void set_current_address(int address, bool notify = true);
	int current_address() const;

	void define_symbol(const Symbol& symbol, const Word& value);
	Word query_defined_symbol(const Symbol& symbol, int near_address) const;

	bool is_defined_symbol(const Symbol& symbol, int near_address) const;

private:
	WordField evaluate_wvalue_field(const std::optional<Expression>& field_expr) const;

	void process_wvalue_token(const WValue::Token& token, Word& dest) const;
	Byte process_ALF_text_char(char ch) const;

	void define_label_if_valid(const Label& label, const Word& value);
	void define_usual_symbol(const Symbol& symbol, const Word& value);
	void define_local_symbol(const Symbol& symbol, int address);

	Word query_usual_symbol(const Symbol& symbol) const;
	Word query_local_symbol(const Symbol& symbol, int near_address) const;

	const int* find_local_symbol(const Symbol& symbol, int near_address) const;

	bool is_defined_usual_symbol(const Symbol& symbol) const;
	bool is_defined_local_symbol(const Symbol& symbol, int near_address) const;

	void increase_current_address();

	void prepare_local_addresses(Addresses& addresses) const;
	void prepare_local_addresses(DefinedLocalSymbols& local_symbols) const;

	AddressTransformation transform_address(const Address& address);

	Byte index_to_byte(const Index& index, const OperationInfo& op_info) const;
	Byte field_to_byte(const Field& field, const OperationInfo& op_info) const;

	FutureTranslatedWordRef process_mix_translation(
		FutureTranslatedWordShared&& partial_result,
		const Address& address, Byte I, Byte F, Byte C);

	int evaluate_address(const Address& address) const;

	Word make_mix_command(int address, Byte I, Byte F, Byte C) const;

	void update_unresolved_references();
	bool try_resolve_previous_word(FutureTranslatedWord& translation_word);
	void resolve_previous_word(FutureTranslatedWord& translation_word);

	std::string_view make_constant(const WValue& wvalue);

	std::vector<Symbol> collect_unresolved_symbols() const;
	FlatMap<Symbol, Word> evaluate_unresolved_symbols(std::vector<Symbol> symbols) const;
	FlatMap<Symbol, Word> evaluate_constants() const;

private:
	struct ChangeTemporaryCurrentAddress;

private:
	int current_address_;

	DefinedSymbols defined_symbols_;
	DefinedLocalSymbols defined_local_symbols_;

	std::vector<FutureTranslatedWordShared> unresolved_words_;

	// Note: using this as a storage for created on the heap
	// runtime strings for names of internal constants.
	// These strings are referenced by `Expression` of `Address`
	// after transformation (see `transform_address()`)
	std::list<std::string> constants_storage_;
	FlatMap<Symbol, WValue> constant_to_value_;
};

struct Translator::Impl::ChangeTemporaryCurrentAddress
{
	ChangeTemporaryCurrentAddress(Translator::Impl& impl, int new_address)
		: impl{impl}
		, original_address{impl.current_address()}
	{
		impl.set_current_address(new_address, false/*do not notify*/);
	}

	~ChangeTemporaryCurrentAddress()
	{
		impl.set_current_address(original_address, false/*do not notify*/);
	}

private:
	Translator::Impl& impl;
	int original_address;
};

Translator::Translator(const DefinedSymbols& symbols /*= {}*/,
	const DefinedLocalSymbols& local_symbols /*= {}*/,
	int current_address /*= 0*/)
		: impl{std::make_unique<Impl>(symbols, local_symbols, current_address)}
{
}

Translator::~Translator() = default;

Word Translator::evaluate(const Text& text) const
{
	return impl->evaluate(text);
}

Word Translator::evaluate(const Symbol& symbol) const
{
	return impl->evaluate(symbol);
}

Word Translator::evaluate(const Number& n) const
{
	return impl->evaluate(n);
}

Word Translator::evaluate(const BasicExpression& expr) const
{
	return impl->evaluate(expr);
}

Word Translator::evaluate(const Expression& expr) const
{
	return impl->evaluate(expr);
}

Word Translator::evaluate(const WValue& wvalue) const
{
	return impl->evaluate(wvalue);
}

FutureTranslatedWordRef Translator::translate_MIX(
	Operation command,
	const Address& A, const Index& I, const Field& F,
	const Label& label /*= {}*/)
{
	return impl->translate_MIX(command, A, I, F, label);
}

void Translator::translate_EQU(const WValue& value, const Label& label /*= {}*/)
{
	return impl->translate_EQU(value, label);
}

void Translator::translate_ORIG(const WValue& address, const Label& label /*= {}*/)
{
	return impl->translate_ORIG(address, label);
}

TranslatedWord Translator::translate_CON(const WValue& address, const Label& label /*= {}*/)
{
	return impl->translate_CON(address, label);
}

TranslatedWord Translator::translate_ALF(const Text& text, const Label& label /*= {}*/)
{
	return impl->translate_ALF(text, label);
}

Translator::EndCommandGeneratedCode Translator::translate_END(
	const WValue& address, const Label& label /*= {}*/)
{
	return impl->translate_END(address, label);
}

void Translator::set_current_address(int address)
{
	return impl->set_current_address(address);
}

int Translator::current_address() const
{
	return impl->current_address();
}

void Translator::define_symbol(const Symbol& symbol, const Word& value)
{
	return impl->define_symbol(symbol, value);
}

Word Translator::query_defined_symbol(const Symbol& symbol, int near_address /*= -1*/) const
{
	return impl->query_defined_symbol(symbol, near_address);
}

bool Translator::is_defined_symbol(const Symbol& symbol, int near_address /*= -1*/) const
{
	return impl->is_defined_symbol(symbol, near_address);
}

Word Translator::Impl::evaluate(const Text& text) const
{
	const auto& data = text.data();
	if (data.size() != Word::k_bytes_count)
	{
		throw InvalidALFText{};
	}

	Word::BytesArray bytes;
	std::transform(data.cbegin(), data.cend(), bytes.begin(),
		[&](char ch)
	{
		return process_ALF_text_char(ch);
	});

	return bytes;
}

Byte Translator::Impl::process_ALF_text_char(char ch) const
{
	bool converted = false;
	const auto char_byte = mix::CharToByte(ch, &converted);
	if (!converted)
	{
		throw InvalidALFText{};
	}
	return char_byte;
}

Word Translator::Impl::evaluate(const BasicExpression& expr) const
{
	if (expr.is_current_address())
	{
		return current_address();
	}
	else if (expr.is_number())
	{
		return evaluate(expr.as_number());
	}
	else if (expr.is_symbol())
	{
		return evaluate(expr.as_symbol());
	}

	// #TODO: add UNREACHABLE() or similar macro
	assert(false);
	return {};
}

Word Translator::Impl::evaluate(const WValue& wvalue) const
{
	assert(wvalue.is_valid());

	Word value;
	for (const auto& token : wvalue.tokens())
	{
		process_wvalue_token(token, value);
	}

	return value;
}

void Translator::Impl::process_wvalue_token(const WValue::Token& token, Word& dest) const
{
	const auto part = evaluate(token.expression);
	const auto field = evaluate_wvalue_field(token.field);

	// Note: part of `CommandProcessor::do_store()` implementation
	const bool take_value_sign = field.includes_sign();
	dest.set_value(
		part.value(field.shift_bytes_right(), take_value_sign),
		field,
		false/*do not overwrite sign*/);
}

WordField Translator::Impl::evaluate_wvalue_field(const std::optional<Expression>& field_expr) const
{
	WordField field = Word::MaxField();
	if (field_expr)
	{
		const auto value = evaluate(*field_expr).value();
		field = WordField::FromByte(int{value});
	}

	if ((field.left() > Word::k_bytes_count) ||
		(field.right() > Word::k_bytes_count))
	{
		throw InvalidWValueField{};
	}
	return field;
}

Word Translator::Impl::evaluate(const Expression& expr) const
{
	assert(expr.is_valid());
	const auto& tokens = expr.tokens();

	auto left_token = tokens[0];
	auto value = CalculateOptionalUnaryOperation(
		left_token.unary_op, evaluate(left_token.basic_expr));

	for (std::size_t i = 1, count = tokens.size(); i < count; ++i)
	{
		auto right_token = tokens[i];
		value = CalculateBinaryOperation(
			*left_token.binary_op, std::move(value), evaluate(right_token.basic_expr));

		left_token = right_token;
	}

	return value;
}

Word Translator::Impl::evaluate(const Number& n) const
{
	const auto v = n.value();
	if (v > Word::k_max_abs_value)
	{
		throw TooBigWordValueError{n};
	}

	return static_cast<int>(v);
}

Word Translator::Impl::evaluate(const Symbol& symbol) const
{
	return query_defined_symbol(symbol, current_address());
}

void Translator::Impl::set_current_address(int address, bool /*notify*/ /*= true*/)
{
	assert(address >= 0);
	current_address_ = address;
}

int Translator::Impl::current_address() const
{
	return current_address_;
}

void Translator::Impl::define_symbol(const Symbol& symbol, const Word& value)
{
	if (symbol.is_local())
	{
		define_local_symbol(symbol, value.value());
	}
	else
	{
		define_usual_symbol(symbol, value);
	}

	update_unresolved_references();
}

void Translator::Impl::define_usual_symbol(const Symbol& symbol, const Word& value)
{
	const auto it = defined_symbols_.insert(std::make_pair(symbol, value));
	const bool inserted = it.second;
	if (!inserted)
	{
		throw DuplicateSymbolDefinitionError{symbol, value};
	}
}

void Translator::Impl::define_local_symbol(const Symbol& symbol, int address)
{
	if (symbol.kind() != LocalSymbolKind::Here)
	{
		throw InvalidLocalSymbolDefinition{symbol};
	}
	const auto id = symbol.local_id();
	auto& addresses = defined_local_symbols_[id];
	addresses.push_back(address);
	prepare_local_addresses(addresses);
}

void Translator::Impl::prepare_local_addresses(Addresses& addresses) const
{
	sort(addresses.begin(), addresses.end());
}

void Translator::Impl::prepare_local_addresses(DefinedLocalSymbols& local_symbols) const
{
	for (auto& addresses : local_symbols)
	{
		prepare_local_addresses(addresses.second);
	}
}

Word Translator::Impl::query_defined_symbol(const Symbol& symbol, int near_address) const
{
	if (symbol.is_local())
	{
		return query_local_symbol(symbol, near_address);
	}
	else
	{
		return query_usual_symbol(symbol);
	}
}

Word Translator::Impl::query_usual_symbol(const Symbol& symbol) const
{
	auto it = defined_symbols_.find(symbol);
	if (it == defined_symbols_.end())
	{
		throw UndefinedSymbolError{symbol};
	}

	return it->second;
}

Word Translator::Impl::query_local_symbol(const Symbol& symbol, int near_address) const
{
	const int* value = nullptr;
	switch (symbol.kind())
	{
	case LocalSymbolKind::Backward:
	case LocalSymbolKind::Forward:
		value = find_local_symbol(symbol, near_address);
		break;
	default: break;
	}

	if (!value)
	{
		throw InvalidLocalSymbolReference{symbol};
	}

	return *value;
}

const int* Translator::Impl::find_local_symbol(const Symbol& symbol, int near_address) const
{
	const auto addresses_it = defined_local_symbols_.find(symbol.local_id());
	if (addresses_it == defined_local_symbols_.end())
	{
		return nullptr;
	}

	// Note: depends on sort criteria in `prepare_local_addresses()`
	const auto& addresses = addresses_it->second;
	if (symbol.kind() == LocalSymbolKind::Backward)
	{
		auto it = lower_bound(addresses.crbegin(), addresses.crend(), near_address, std::greater<>{});
		return (it != addresses.crend()) ? &*it : nullptr;
	}
	else if (symbol.kind() == LocalSymbolKind::Forward)
	{
		auto it = lower_bound(addresses.cbegin(), addresses.cend(), near_address);
		return (it != addresses.cend()) ? &*it : nullptr;
	}

	return nullptr;
}

bool Translator::Impl::is_defined_symbol(const Symbol& symbol, int near_address) const
{
	if (symbol.is_local())
	{
		return is_defined_local_symbol(symbol, near_address);
	}
	else
	{
		return is_defined_usual_symbol(symbol);
	}
}

bool Translator::Impl::is_defined_usual_symbol(const Symbol& symbol) const
{
	return (defined_symbols_.find(symbol) != defined_symbols_.end());
}

bool Translator::Impl::is_defined_local_symbol(const Symbol& symbol, int near_address) const
{
	switch (symbol.kind())
	{
	case LocalSymbolKind::Backward:
	case LocalSymbolKind::Forward:
		return (find_local_symbol(symbol, near_address) != nullptr);
	default: break;
	};
	
	return false;
}

void Translator::Impl::define_label_if_valid(const Label& label, const Word& value)
{
	if (label.empty())
	{
		return;
	}

	define_symbol(label.symbol(), value);
}

void Translator::Impl::translate_EQU(const WValue& value, const Label& label)
{
	define_label_if_valid(label, evaluate(value));
}

void Translator::Impl::translate_ORIG(const WValue& value, const Label& label)
{
	const auto address = evaluate(value);
	define_label_if_valid(label, address);
	set_current_address(address.value());
}

TranslatedWord Translator::Impl::translate_CON(const WValue& wvalue, const Label& label)
{
	return translate_CON(evaluate(wvalue), label);
}

TranslatedWord Translator::Impl::translate_CON(const Word& value, const Label& label)
{
	const auto address = current_address();
	define_label_if_valid(label, address);

	TranslatedWord result{address, value};
	increase_current_address();
	return result;
}

TranslatedWord Translator::Impl::translate_ALF(const Text& text, const Label& label)
{
	const auto address = current_address();
	define_label_if_valid(label, address);

	TranslatedWord result{address, evaluate(text)};
	increase_current_address();
	return result;
}

// Note: this function _may be_ not exception safe. This means
// that after any kind of exception/error `Translator` state will be in
// undetermined state (TODO: investigate)
Translator::EndCommandGeneratedCode Translator::Impl::translate_END(
	const WValue& value, const Label& label)
{
	const auto symbols = CombineFlatMaps(
		evaluate_constants(),
		evaluate_unresolved_symbols(
			collect_unresolved_symbols()));

	EndCommandGeneratedCode generated_code;
	generated_code.program_start_address = evaluate(value).value();

	for (const auto& symbol_value : symbols)
	{
		const auto symbol = symbol_value.first;
		const auto translated = translate_CON(symbol_value.second, symbol);

		generated_code.undefined_symbols.push_back(
			std::make_pair(symbol, translated));
	}

	define_label_if_valid(label, current_address());
	return generated_code;
}

FutureTranslatedWordRef Translator::Impl::translate_MIX(
	Operation command,
	const Address& address, const Index& index, const Field& field,
	const Label& label)
{
	const auto command_info = QueryOperationInfo(command);
	Byte C = command_info.computer_command;
	Byte I = index_to_byte(index, command_info);
	Byte F = field_to_byte(field, command_info);

	const auto original_address = current_address();
	const auto transformation = transform_address(address);
	auto future_word = std::make_shared<FutureTranslatedWord>(
		original_address,
		transformation.forward_references);

	define_label_if_valid(label, original_address);

	auto result = process_mix_translation(
		std::move(future_word), transformation.address, I, F, C);

	// Note: when resolving forwarding references later and translating
	// `Address` expression to its value, we should replace
	// `current_address()` with `original_address` to resolve
	// expressions like `**X` in the past (since `*` will be changed)
	increase_current_address();
	return result;
}

void Translator::Impl::increase_current_address()
{
	set_current_address(current_address() + 1);
}

AddressTransformation Translator::Impl::transform_address(const Address& address)
{
	AddressTransformation transformed;
	
	auto collect_from_expression_token = [&](const Expression::Token& token)
	{
		if (!token.basic_expr.is_symbol())
		{
			return;
		}

		const auto& symbol = token.basic_expr.as_symbol();

		const bool is_forward_symbol = (symbol.kind() == LocalSymbolKind::Forward);
		const bool is_undefined_usual_symbol =
			(symbol.kind() == LocalSymbolKind::Usual) &&
			!is_defined_symbol(symbol, -1/*ignore address*/);

		if (is_forward_symbol || is_undefined_usual_symbol)
		{
			transformed.forward_references.push_back(symbol);
		}
	};

	auto collect_from_expression = [&](const Expression& expr)
	{
		for (const auto& token : expr.tokens())
		{
			collect_from_expression_token(token);
		}
	};

	if (address.has_expression())
	{
		collect_from_expression(address.expression());
		transformed.address = address;
	}
	else if (address.has_literal_constant())
	{
		const auto const_name = make_constant(address.w_value());
		transformed.forward_references.push_back(const_name);
		transformed.address = Address{MakeExression(const_name)};
	}

	return transformed;
}

std::string_view Translator::Impl::make_constant(const WValue& wvalue)
{
	const auto id = constants_storage_.size() + 1;
	constants_storage_.push_back("@CON" + std::to_string(id));
	std::string_view name = constants_storage_.back();
	constant_to_value_.push_back(std::make_pair(Symbol{name}, wvalue));
	return name;
}

Byte Translator::Impl::index_to_byte(const Index& index, const OperationInfo& op_info) const
{
	if (index.empty())
	{
		return op_info.default_index;
	}
	const auto value = evaluate(index.expression()).value();
	return int{value};
}

Byte Translator::Impl::field_to_byte(const Field& field, const OperationInfo& op_info) const
{
	if (field.empty())
	{
		return op_info.default_field.to_byte();
	}

	const auto value = evaluate(field.expression()).value();
	return int{value};
}

int Translator::Impl::evaluate_address(const Address& address) const
{
	if (address.has_expression())
	{
		// Note: `address` shoud not refer to forwarding symbol
		// (undefined symbol exception will be populated otherwice)
		return evaluate(address.expression()).value();
	}
	else if (address.has_w_value())
	{
		// Address's Literal Contant should be already processed before
		// and replaced to Forward Reference (see `transform_address()` function)
		assert(false);
		return 0;
	}

	return 0;
}

Word Translator::Impl::make_mix_command(int address, Byte I, Byte F, Byte C) const
{
	// #TODO: check address/index and so on validness
	const mix::Command command{
		C.cast_to<std::size_t>(),
		address,
		I.cast_to<std::size_t>(),
		WordField::FromByte(F)};

	return command.to_word();
}

FutureTranslatedWordRef Translator::Impl::process_mix_translation(
	FutureTranslatedWordShared&& partial_result,
	const Address& address, Byte I, Byte F, Byte C)
{
	if (partial_result->is_ready())
	{
		partial_result->value = make_mix_command(
			evaluate_address(address), I, F, C);
		return partial_result;
	}

	partial_result->value = make_mix_command(0, I, F, C);
	partial_result->unresolved_address = address;
	unresolved_words_.push_back(partial_result);
	return partial_result;
}

void Translator::Impl::update_unresolved_references()
{
	// Can't use generic lambda since bug in GCC 4.8.4 (Travis CI)
	// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=61636
	unresolved_words_.erase(remove_if(begin(unresolved_words_), end(unresolved_words_),
		[&](const FutureTranslatedWordShared& future_word)
	{
		return try_resolve_previous_word(*future_word);
	}) , end(unresolved_words_));
}

bool Translator::Impl::try_resolve_previous_word(FutureTranslatedWord& translation_word)
{
	auto& references = translation_word.forward_references;
	const auto original_address = translation_word.original_address;

	references.erase(remove_if(begin(references), end(references),
		[&](const Symbol& symbol)
	{
		return is_defined_symbol(symbol, original_address);
	}), end(references));

	if (translation_word.is_ready())
	{
		resolve_previous_word(translation_word);
		return true;
	}

	return false;
}

void Translator::Impl::resolve_previous_word(FutureTranslatedWord& translation_word)
{
	assert(translation_word.is_ready());
	const auto original_address = translation_word.original_address;
	ChangeTemporaryCurrentAddress _{*this, original_address};
	mix::Command command{translation_word.value};
	command.change_address(evaluate_address(translation_word.unresolved_address));
	translation_word.value = command.to_word();
}

Translator::Impl::Impl(const DefinedSymbols& symbols,
	const DefinedLocalSymbols& local_symbols,
	int current_address)
		: current_address_{current_address}
		, defined_symbols_{symbols}
		, defined_local_symbols_{local_symbols}
{
	prepare_local_addresses(defined_local_symbols_);
}

std::vector<Symbol> Translator::Impl::collect_unresolved_symbols() const
{
	// Flatten all forward references of all unresolved words into single array
	std::vector<Symbol> symbols;
	for (const auto& unresolved_word : unresolved_words_)
	{
		copy(unresolved_word->forward_references.cbegin(),
			unresolved_word->forward_references.cend(),
			back_inserter(symbols));
	}
	return symbols;
}

FlatMap<Symbol, Word> Translator::Impl::evaluate_unresolved_symbols(
	std::vector<Symbol> symbols) const
{
	const Word k_undefined_symbol_value{0};

	FlatMap<Symbol, Word> symbol_to_value;
	for (auto symbol : symbols)
	{
		symbol_to_value.push_back(std::make_pair(symbol, k_undefined_symbol_value));
	}
	return symbol_to_value;
}

FlatMap<Symbol, Word> Translator::Impl::evaluate_constants() const
{
	FlatMap<Symbol, Word> symbol_to_value;
	for (const auto& constant_wvalue : constant_to_value_)
	{
		symbol_to_value.push_back(std::make_pair(constant_wvalue.first,
			evaluate(constant_wvalue.second)));
	}
	return symbol_to_value;
}

