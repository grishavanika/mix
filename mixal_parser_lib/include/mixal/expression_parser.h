#include <mixal/parser.h>
#include <mixal/parsers_utils.h>

#include <memory>
#include <optional>
#include <vector>

namespace mixal {

class ExpressionParser final :
	public IParser
{
public:
	struct Token
	{
		std::optional<UnaryOperation> unary_op;
		BasicExpression basic_expr;
		std::optional<BinaryOperation> binary_op;
	};

public:
	virtual void parse(std::string_view str) override;
	virtual std::string_view str() const override;

	const std::vector<Token>& tokens() const;

private:
	void clear();
	void parse_basic_expr_with_optional_unary_op();
	void parse_basic_expr_with_unary_op();
	void parse_basic_expr_with_binary_op();
	void parse_single_basic_expr();

	bool eof() const;
	char move_to_next_char();
	char current_char() const;
	void skip_white_spaces();

	void finish_current_token();

	[[noreturn]] void throw_error(const char* details) const;

	template<typename CharPredicate, typename CompletenessPredicate>
	std::string_view build_non_empty_expr(CharPredicate pred, CompletenessPredicate completed);

private:
	std::vector<Token> tokens_;
	std::vector<Token> final_tokens_;
	std::string_view parse_str_;
	std::size_t parse_pos_;
	Token current_token_;
};

bool operator==(const ExpressionParser::Token& lhs, const ExpressionParser::Token& rhs);

} // namespace mixal

#include <mixal/internal/expression_parser.inl>
