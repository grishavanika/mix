#pragma once
#include <mixal/config.h>
#include <mixal/types.h>

#include <map>
#include <vector>
#include <memory>

namespace mixal {

struct OperationInfo;

class MIXAL_LIB_EXPORT Translator
{
public:
	using DefinedSymbols		= std::map<Symbol, Word>;
	using Addresses				= std::vector<int/*addresses*/>;
	using DefinedLocalSymbols	= std::map<LocalSymbolId, Addresses>;

public:
	Translator(const DefinedSymbols& symbols = {},
		const DefinedLocalSymbols& local_symbols = {},
		int current_address = 0);
	
	~Translator();

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
	
	TranslatedWord translate_CON(const WValue& address, const Label& label = {});
	TranslatedWord translate_ALF(const Text& text, const Label& label = {});
	void translate_END(const WValue& address, const Label& label = {});

	void set_current_address(int address);
	int current_address() const;

	void define_symbol(const Symbol& symbol, const Word& value);
	Word query_defined_symbol(const Symbol& symbol, int near_address = -1) const;

	bool is_defined_symbol(const Symbol& symbol, int near_address = -1) const;

private:
	struct Impl;
	std::unique_ptr<Impl> impl;
};

} // namespace mixal


