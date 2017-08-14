#include <mixal/operation_parser.h>
#include <mixal/parse_exceptions.h>

#include <gtest/gtest.h>

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
	public ::testing::TestWithParam<OperationIdParam>
{
};

} // namespace

TEST(OperationParser, Does_Not_Ignore_WhiteSpaces)
{
	ASSERT_THROW({
		OperationParser parser;
		parser.parse("   ADD");
	}, UnknownOperationId);

	ASSERT_THROW({
		OperationParser parser;
		parser.parse("SUB   ");
	}, UnknownOperationId);
}

TEST(OperationParser, Accepts_Only_All_Uppercase)
{
	ASSERT_THROW({
		OperationParser parser;
		parser.parse("sta");
	}, UnknownOperationId);

	OperationParser parser;
	parser.parse("STA");
	ASSERT_EQ(OperationId::STA, parser.id());
}

TEST(OperationParser, Throws_UnknownOperationId_On_Empty_String)
{
	ASSERT_THROW({
		OperationParser parser;
		parser.parse("");
	}, UnknownOperationId);
}

TEST_P(OperationParserTest, Does_Not_Throw_For_Known_UpperCase_Operations)
{
	const auto& param = GetParam();

	OperationParser parser;
	ASSERT_NO_THROW({
		parser.parse(param.str);
	});

	ASSERT_EQ(param.id, parser.id());
}

TEST_P(OperationParserTest, Has_Valid_Pseudo_Or_Native_Operation_Flags)
{
	const auto& param = GetParam();

	OperationParser parser;
	ASSERT_NO_THROW({
		parser.parse(param.str);
	});

	switch (param.type)
	{
	case OperationType::Native:
		ASSERT_TRUE(parser.is_native_operation());
		ASSERT_FALSE(parser.is_pseudo_operation());
		break;
	case OperationType::Pseudo:
		ASSERT_TRUE(parser.is_pseudo_operation());
		ASSERT_FALSE(parser.is_native_operation());
		break;
	}
}

INSTANTIATE_TEST_CASE_P(ValidOperationStrings,
	OperationParserTest,
	::testing::Values(
		/*00*/OperationIdParam{OperationId::ADD, OperationType::Native, "ADD"},
		/*01*/OperationIdParam{OperationId::MUL, OperationType::Native, "MUL"},
		/*02*/OperationIdParam{OperationId::LDAN, OperationType::Native, "LDAN"},
		/*03*/OperationIdParam{OperationId::IOC, OperationType::Native, "IOC"},
		/*04*/OperationIdParam{OperationId::CON, OperationType::Pseudo, "CON"},
		/*05*/OperationIdParam{OperationId::ORIG, OperationType::Pseudo, std::string_view{"ORIG xxxx", 4}}
		));


