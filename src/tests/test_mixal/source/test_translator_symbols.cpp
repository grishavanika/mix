#include <mixal/translator.h>
#include <mixal/exceptions.h>

#include <gtest_all.h>

using namespace mixal;
using namespace mix;
using namespace mixal_parse;

namespace {

} // namespace

TEST(TranslatorSymbolsTest, Usual_Symbol_Can_Be_Queried_After_Definition)
{
	Translator translator;
	translator.define_symbol("X", 42);
	ASSERT_TRUE(translator.is_defined_symbol("X"));
	ASSERT_EQ(42, translator.query_defined_symbol("X"));
}

TEST(TranslatorSymbolsTest, Usual_Symbol_Can_Be_Definition_Only_Once)
{
	Translator translator;

	ASSERT_NO_THROW({
		translator.define_symbol("X", 0);
	});

	ASSERT_THROW({
		translator.define_symbol("X", 1);
	}, DuplicateSymbolDefinitionError);

	ASSERT_EQ(0, translator.query_defined_symbol("X"));
}

TEST(TranslatorSymbolsTest, Queriing_Undefined_Usual_Symbol_Throws_UndefinedSymbolError)
{
	Translator translator;
	ASSERT_THROW({
		translator.query_defined_symbol("Y");
	}, UndefinedSymbolError);
}

TEST(TranslatorSymbolsTest, Asking_Undefined_Usual_Symbol_Returns_False)
{
	Translator translator;
	ASSERT_FALSE(translator.is_defined_symbol("Y"));
	translator.define_symbol("Y", 0);
	ASSERT_TRUE(translator.is_defined_symbol("Y"));
}

TEST(TranslatorSymbolsTest, Only_HERE_Local_Symbol_Can_Be_Defined)
{
	Translator translator;

	ASSERT_THROW({
		translator.define_symbol(Symbol::FromString("2B"), 100);
	}, InvalidLocalSymbolDefinition);

	ASSERT_THROW({
		translator.define_symbol(Symbol::FromString("2F"), 1000);
	}, InvalidLocalSymbolDefinition);

	translator.define_symbol(Symbol::FromString("2H"), 1000);
}

TEST(TranslatorSymbolsTest, BACKWARD_Local_Symbol_Can_Be_Queried)
{
	Translator translator;
	translator.define_symbol(Symbol::FromString("2H"), 1000);
	
	ASSERT_FALSE(translator.is_defined_symbol(Symbol::FromString("2B"), 33));
	ASSERT_TRUE(translator.is_defined_symbol(Symbol::FromString("2B"), 1111));
}

TEST(TranslatorSymbolsTest, HERE_Local_Symbol_Can_NOT_Be_Queried)
{
	Translator translator;
	translator.define_symbol(Symbol::FromString("2H"), 1000);

	ASSERT_FALSE(translator.is_defined_symbol(Symbol::FromString("2H"), 1000));
}

TEST(TranslatorSymbolsTest, FORWARD_Local_Symbol_Can_Be_Queried)
{
	Translator translator;
	translator.define_symbol(Symbol::FromString("2H"), 1000);

	ASSERT_TRUE(translator.is_defined_symbol(Symbol::FromString("2F"), 33));
	ASSERT_FALSE(translator.is_defined_symbol(Symbol::FromString("2F"), 1111));
}

TEST(TranslatorSymbolsTest, Multiple_HERE_Local_Symbols_Can_Be_Defined)
{
	Translator translator;

	translator.define_symbol(Symbol::FromString("2H"), 1000);
	translator.define_symbol(Symbol::FromString("2H"), 0);

	translator.define_symbol(Symbol::FromString("9H"), 100);
	translator.define_symbol(Symbol::FromString("9H"), 10);
}

TEST(TranslatorSymbolsTest, Backward_Local_Symbol_Value_Depends_On_Queried_Address_Location)
{
	Translator translator;

	translator.define_symbol(Symbol::FromString("2H"), 100);
	translator.define_symbol(Symbol::FromString("2H"), 1000);

	ASSERT_EQ(100, translator.query_defined_symbol(Symbol::FromString("2B"), 200));
	ASSERT_EQ(1000, translator.query_defined_symbol(Symbol::FromString("2B"), 2000));
}

TEST(TranslatorSymbolsTest, Forward_Local_Symbol_Value_Depends_On_Queried_Address_Location)
{
	Translator translator;

	translator.define_symbol(Symbol::FromString("2H"), 100);
	translator.define_symbol(Symbol::FromString("2H"), 1000);

	ASSERT_EQ(1000, translator.query_defined_symbol(Symbol::FromString("2F"), 200));
	ASSERT_EQ(100, translator.query_defined_symbol(Symbol::FromString("2F"), 20));
}

TEST(TranslatorSymbolsTest, Asking_Undefined_Backward_Symbol_Returns_False)
{
	Translator translator;

	ASSERT_FALSE(translator.is_defined_symbol(Symbol::FromString("7B"), 200));
	translator.define_symbol(Symbol::FromString("7H"), 100);
	ASSERT_TRUE(translator.is_defined_symbol(Symbol::FromString("7B"), 200));
}

TEST(TranslatorSymbolsTest, Backward_Local_Symbol_Value_Depends_On_Local_Id)
{
	Translator translator;

	translator.define_symbol(Symbol::FromString("7H"), 100);
	translator.define_symbol(Symbol::FromString("3H"), 300);
	
	ASSERT_THROW({
		translator.query_defined_symbol(Symbol::FromString("4B"), 400);
	}, InvalidLocalSymbolReference);

	ASSERT_EQ(300, translator.query_defined_symbol(Symbol::FromString("3B"), 400));
	ASSERT_EQ(100, translator.query_defined_symbol(Symbol::FromString("7B"), 400));
}


