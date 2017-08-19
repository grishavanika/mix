#pragma once
#include <mixal/config.h>
#include <mixal/types.h>

namespace mixal {

class MIXAL_LIB_EXPORT Translator
{
public:
	Translator(int current_address = 0);

	Word evaluate(const Text& text) const;
	Word evaluate(const Symbol& symbol) const;
	Word evaluate(const Number& n) const;
	Word evaluate(const Expression& expr) const;
	Word evaluate(const WValue& wvalue) const;

	FutureWord translate_MIX(
		Operation command,
		const APart& A, const IPart& I, const FPart& F,
		const Label& label = {});

	void translate_EQU(const WValue& value, const Label& label = {});
	void translate_ORIG(const WValue& address, const Label& label = {});
	void translate_CON(const WValue& address, const Label& label = {});
	void translate_ALF(const Text& text, const Label& label = {});
	void translate_END(const WValue& address, const Label& label = {});

	void set_current_address(int address);
	int current_address() const;

private:
	int current_address_;
};

} // namespace mixal


