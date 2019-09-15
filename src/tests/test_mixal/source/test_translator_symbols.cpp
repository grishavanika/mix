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
	translator.define_symbol("X", Word(42));
	ASSERT_TRUE(translator.is_defined_symbol("X"));
	ASSERT_EQ(Word(42), translator.query_defined_symbol("X"));
}

TEST(TranslatorSymbolsTest, Usual_Symbol_Can_Be_Definition_Only_Once)
{
	Translator translator;

	ASSERT_NO_THROW({
		translator.define_symbol("X", Word(0));
	});

	ASSERT_THROW({
		translator.define_symbol("X", Word(1));
	}, DuplicateSymbolDefinitionError);

	ASSERT_EQ(Word(0), translator.query_defined_symbol("X"));
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
	translator.define_symbol("Y", Word(0));
	ASSERT_TRUE(translator.is_defined_symbol("Y"));
}

TEST(TranslatorSymbolsTest, Only_HERE_Local_Symbol_Can_Be_Defined)
{
	Translator translator;

	ASSERT_THROW({
		translator.define_symbol(Symbol::FromString("2B"), Word(100));
	}, InvalidLocalSymbolDefinition);

	ASSERT_THROW({
		translator.define_symbol(Symbol::FromString("2F"), Word(1000));
	}, InvalidLocalSymbolDefinition);

	translator.define_symbol(Symbol::FromString("2H"), Word(1000));
}

TEST(TranslatorSymbolsTest, BACKWARD_Local_Symbol_Can_Be_Queried)
{
	Translator translator;
	translator.define_symbol(Symbol::FromString("2H"), Word(1000));
	
	ASSERT_FALSE(translator.is_defined_symbol(Symbol::FromString("2B"), 33));
	ASSERT_TRUE(translator.is_defined_symbol(Symbol::FromString("2B"), 1111));
}

TEST(TranslatorSymbolsTest, HERE_Local_Symbol_Can_NOT_Be_Queried)
{
	Translator translator;
	translator.define_symbol(Symbol::FromString("2H"), Word(1000));

	ASSERT_FALSE(translator.is_defined_symbol(Symbol::FromString("2H"), 1000));
}

TEST(TranslatorSymbolsTest, FORWARD_Local_Symbol_Can_Be_Queried)
{
	Translator translator;
	translator.define_symbol(Symbol::FromString("2H"), Word(1000));

	ASSERT_TRUE(translator.is_defined_symbol(Symbol::FromString("2F"), 33));
	ASSERT_FALSE(translator.is_defined_symbol(Symbol::FromString("2F"), 1111));
}

TEST(TranslatorSymbolsTest, Multiple_HERE_Local_Symbols_Can_Be_Defined)
{
	Translator translator;

	translator.define_symbol(Symbol::FromString("2H"), Word(1000));
	translator.define_symbol(Symbol::FromString("2H"), Word(0));

	translator.define_symbol(Symbol::FromString("9H"), Word(100));
	translator.define_symbol(Symbol::FromString("9H"), Word(10));
}

TEST(TranslatorSymbolsTest, Backward_Local_Symbol_Value_Depends_On_Queried_Address_Location)
{
	Translator translator;

	translator.define_symbol(Symbol::FromString("2H"), Word(100));
	translator.define_symbol(Symbol::FromString("2H"), Word(1000));

	ASSERT_EQ(Word(100), translator.query_defined_symbol(Symbol::FromString("2B"), 200));
	ASSERT_EQ(Word(1000), translator.query_defined_symbol(Symbol::FromString("2B"), 2000));
}

TEST(TranslatorSymbolsTest, Forward_Local_Symbol_Value_Depends_On_Queried_Address_Location)
{
	Translator translator;

	translator.define_symbol(Symbol::FromString("2H"), Word(100));
	translator.define_symbol(Symbol::FromString("2H"), Word(1000));

	ASSERT_EQ(Word(1000), translator.query_defined_symbol(Symbol::FromString("2F"), 200));
	ASSERT_EQ(Word(100), translator.query_defined_symbol(Symbol::FromString("2F"), 20));
}

TEST(TranslatorSymbolsTest, Asking_Undefined_Backward_Symbol_Returns_False)
{
	Translator translator;

	ASSERT_FALSE(translator.is_defined_symbol(Symbol::FromString("7B"), 200));
	translator.define_symbol(Symbol::FromString("7H"), Word(100));
	ASSERT_TRUE(translator.is_defined_symbol(Symbol::FromString("7B"), 200));
}

TEST(TranslatorSymbolsTest, Backward_Local_Symbol_Value_Depends_On_Local_Id)
{
	Translator translator;

	translator.define_symbol(Symbol::FromString("7H"), Word(100));
	translator.define_symbol(Symbol::FromString("3H"), Word(300));
	
	ASSERT_THROW({
		translator.query_defined_symbol(Symbol::FromString("4B"), 400);
	}, InvalidLocalSymbolReference);

	ASSERT_EQ(Word(300), translator.query_defined_symbol(Symbol::FromString("3B"), 400));
	ASSERT_EQ(Word(100), translator.query_defined_symbol(Symbol::FromString("7B"), 400));
}


