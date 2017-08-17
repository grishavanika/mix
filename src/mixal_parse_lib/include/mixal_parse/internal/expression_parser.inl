
namespace mixal_parse {

template<typename CharPredicate, typename CompletenessPredicate>
std::string_view ExpressionParser::build_non_empty_expr(
	CharPredicate pred,
	CompletenessPredicate completed)
{
	skip_white_spaces();

	const auto start_pos = parse_pos_;

	auto build_result = [=]() -> std::string_view
	{
		const auto end_pos = parse_pos_;
		return parse_str_.substr(start_pos, end_pos - start_pos);
	};

	while (!eof() && pred(current_char()))
	{
		move_to_next_char();
		if (!completed(build_result()))
		{
			put_char_back();
			break;
		}
	}

	const auto result = build_result();
	if (result.empty())
	{
		// #TODO: add caller context to this error
		throw_error("Expecting non-empty expression");
	}

	last_parsed_token_pos_ = parse_pos_;
	return result;
}

} // namespace mixal_parse
