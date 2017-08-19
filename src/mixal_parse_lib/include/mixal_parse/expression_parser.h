#pragma once
#include <mixal_parse/parser_base.h>
#include <mixal_parse/types/expression.h>

namespace mixal_parse {

class MIXAL_PARSE_LIB_EXPORT ExpressionParser final :
	public ParserBase
{
public:
	const Expression& expression() const;

private:
	virtual std::size_t do_parse_stream(std::string_view str, std::size_t offset) override;
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
	std::string_view parse_str_;
	std::size_t parse_pos_;
	std::size_t last_parsed_token_pos_;
	ExpressionToken current_token_;
};

} // namespace mixal_parse

#include <mixal_parse/internal/expression_parser.inl>
