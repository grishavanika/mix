#include <mixal/parser.h>
#include <mixal/expression.h>

namespace mixal {

class ExpressionParser final :
	public IParser
{
public:
	const Expression& expression() const;

private:
	virtual void do_parse(std::string_view str) override;
	virtual void do_clear() override;

	void parse_basic_expr_with_optional_unary_op();
	void parse_basic_expr_with_unary_op();
	void parse_basic_expr_with_binary_op();
	void parse_single_basic_expr();

	bool eof() const;
	char move_to_next_char();
	void put_char_back();
	char current_char() const;
	void skip_white_spaces();

	void finish_current_token();

	[[noreturn]] void throw_error(const char* details) const;

	template<typename CharPredicate, typename CompletenessPredicate>
	std::string_view build_non_empty_expr(CharPredicate pred, CompletenessPredicate completed);

private:
	Expression expression_;
	Expression final_expression_;
	std::string_view parse_str_;
	std::size_t parse_pos_;
	ExpressionToken current_token_;
};

} // namespace mixal

#include <mixal/internal/expression_parser.inl>
