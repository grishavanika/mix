#include <mixal/operation_parser.h>

#include "parser_test_fixture.h"

using namespace mixal;

namespace {

enum class OperationType
{
	Pseudo,
	Native
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
		<< ((param.type == OperationType::Native) ? "native" : "pseudo");
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

TEST_P(OperationParserTestParam, Has_Valid_Pseudo_Or_Native_Operation_Flags)
{
	const auto& param = GetParam();
	parse(param.str);

	switch (param.type)
	{
	case OperationType::Native:
		ASSERT_TRUE(parser_.is_native_operation());
		ASSERT_FALSE(parser_.is_pseudo_operation());
		break;
	case OperationType::Pseudo:
		ASSERT_TRUE(parser_.is_pseudo_operation());
		ASSERT_FALSE(parser_.is_native_operation());
		break;
	}
}

INSTANTIATE_TEST_CASE_P(ValidOperationStrings,
	OperationParserTestParam,
	::testing::Values(
		/*00*/OperationIdParam{OperationId::ADD, OperationType::Native, "ADD"},
		/*01*/OperationIdParam{OperationId::MUL, OperationType::Native, "MUL"},
		/*02*/OperationIdParam{OperationId::LDAN, OperationType::Native, "LDAN"},
		/*03*/OperationIdParam{OperationId::IOC, OperationType::Native, "IOC"},
		/*04*/OperationIdParam{OperationId::CON, OperationType::Pseudo, "CON"},
		/*05*/OperationIdParam{OperationId::ORIG, OperationType::Pseudo, std::string_view{"ORIG xxxx", 4}}
		));


