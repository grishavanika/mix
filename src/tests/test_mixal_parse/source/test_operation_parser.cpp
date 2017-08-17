#include <mixal_parse/operation_parser.h>

#include "parser_test_fixture.h"

using namespace mixal_parse;

namespace {

enum class OperationType
{
	MIX,
	MIXAL
};

struct OperationIdParam
{
	OperationId id;
	OperationType type;
	std::string_view str;
};

std::ostream& operator<<(std::ostream& o, const OperationIdParam& param)
{
	o << "Expect " << param.str << " (" << static_cast<int>(param.id) << ") "
		<< "operation to be "
		<< ((param.type == OperationType::MIX) ? "MIX" : "MIXAL");
	return o;
}

class OperationParserTest :
	public ParserTest<OperationParser>
{
};

class OperationParserTestParam :
	public OperationParserTest,
	public ::testing::WithParamInterface<OperationIdParam>
{
};

} // namespace

TEST_F(OperationParserTest, Ignores_WhiteSpaces)
{
	parse("   ADD");
	reminder_stream_is("");
	ASSERT_EQ(OperationId::ADD, parser_.id());

	parse("SUB   ");
	ASSERT_EQ(OperationId::SUB, parser_.id());
	reminder_stream_is("   ");
}

TEST_F(OperationParserTest, Accepts_Only_All_Uppercase)
{
	parse_error("sta");
}

TEST_F(OperationParserTest, Fails_On_Empty_String)
{
	parse_error("");
}

TEST_P(OperationParserTestParam, Does_Not_Fail_For_Known_UpperCase_Operations)
{
	const auto& param = GetParam();
	parse(param.str);
	ASSERT_EQ(param.id, parser_.id());
}

TEST_P(OperationParserTestParam, Has_Valid_MIX_Or_MIXAL_Operations_Queries)
{
	const auto& param = GetParam();
	parse(param.str);

	switch (param.type)
	{
	case OperationType::MIX:
		ASSERT_TRUE(parser_.is_mix_operation());
		ASSERT_FALSE(parser_.is_mixal_operation());
		break;
	case OperationType::MIXAL:
		ASSERT_TRUE(parser_.is_mixal_operation());
		ASSERT_FALSE(parser_.is_mix_operation());
		break;
	}
}

INSTANTIATE_TEST_CASE_P(ValidOperationStrings,
	OperationParserTestParam,
	::testing::Values(
		/*00*/OperationIdParam{OperationId::ADD, OperationType::MIX, "ADD"},
		/*01*/OperationIdParam{OperationId::MUL, OperationType::MIX, "MUL"},
		/*02*/OperationIdParam{OperationId::LDAN, OperationType::MIX, "LDAN"},
		/*03*/OperationIdParam{OperationId::IOC, OperationType::MIX, "IOC"},
		/*04*/OperationIdParam{OperationId::CON, OperationType::MIXAL, "CON"},
		/*05*/OperationIdParam{OperationId::ORIG, OperationType::MIXAL, std::string_view{"ORIG xxxx", 4}}
		));


