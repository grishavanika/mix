#pragma once
#include <mixal/types.h>

namespace mixal {

////////////////////////////////////////////////////////////////////////////////
// #TODO: names are bad, needed args are not what needed :(
class TranslatorListener
{
	// #TODO: or pass `FutureWord` reference instead of address
	virtual void on_future_word_resolved(int address, .../*?*/) = 0;
	virtual void on_unresolved_word_created(int address, .../*?*/) = 0;
	
	virtual void on_symbol_defined(const Symbol& symbol, int address, const Word& value) = 0;

	virtual void on_local_symbol_defined(int id, int address, const Symbol& symbol, ...) = 0;

	virtual void on_current_address_changed(int prev, int curr) = 0;

	virtual void on_constant_defined(const Symbol& symbol, int address, const Word& value) = 0;
	virtual void on_undefined_symbol_defined(const Symbol& symbol, int address, const Word& value) = 0;

protected:
	~TranslatorListener() = default;
};


} // namespace mixal


