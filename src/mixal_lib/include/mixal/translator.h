#pragma once
#include <mixal/config.h>
#include <mixal/types.h>

#include <map>
#include <vector>

namespace mixal {

struct OperationInfo;

class MIXAL_LIB_EXPORT Translator
{
public:
	using DefinedSymbols		= std::map<Symbol, Word>;
	using Addresses				= std::vector<int/*addresses*/>;
	using DefinedLocalSymbols	= std::map<LocalSymbolId, Addresses>;

public:
	Translator(
		const DefinedSymbols& symbols = {},
		const DefinedLocalSymbols& local_symbols = {},
		int current_address = 0);

	Word evaluate(const Text& text) const;
	Word evaluate(const Symbol& symbol) const;
	Word evaluate(const Number& n) const;
	Word evaluate(const BasicExpression& expr) const;
	Word evaluate(const Expression& expr) const;
	Word evaluate(const WValue& wvalue) const;

	FutureTranslatedWordRef translate_MIX(
		Operation command,
		const Address& A, const Index& I, const Field& F,
		const Label& label = {});

	void translate_EQU(const WValue& value, const Label& label = {});
	void translate_ORIG(const WValue& address, const Label& label = {});
	
	// #TODO: `WValue` can contain `Forward reference`, hence we should return `FutureWord`
	TranslatedWord translate_CON(const WValue& address, const Label& label = {});
	TranslatedWord translate_ALF(const Text& text, const Label& label = {});
	void translate_END(const WValue& address, const Label& label = {});

	void set_current_address(int address);
	int current_address() const;

	void define_symbol(const Symbol& symbol, const Word& value);
	Word query_defined_symbol(const Symbol& symbol, int near_address = -1) const;

	bool is_defined_symbol(const Symbol& symbol, int near_address = -1) const;

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

	std::vector<Symbol> query_address_forward_references(const Address& address) const;

	Byte index_to_byte(const Index& index, const OperationInfo& op_info) const;
	Byte field_to_byte(const Field& field, const OperationInfo& op_info) const;

	FutureTranslatedWordRef process_mix_translation(
		FutureTranslatedWordShared&& partial_result,
		const Address& address, Byte I, Byte F, Byte C);

	int translate_address(const Address& address) const;

	Word make_mix_command(int address, Byte I, Byte F, Byte C) const;

private:
	int current_address_;
	DefinedSymbols defined_symbols_;
	DefinedLocalSymbols defined_local_symbols_;
	std::vector<FutureTranslatedWordShared> unresolved_translations_;
};

} // namespace mixal


