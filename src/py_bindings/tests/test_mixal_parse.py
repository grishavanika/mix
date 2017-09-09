from py_mixal_parse import *

number = Number('42')
print('Number:', number)
print(number.data)
assert number.value == 42
assert not number.is_zero()

symbol = Symbol.from_string('5H')
print('Symbol:', symbol)
print(symbol.name)
assert symbol.is_valid()
assert symbol.is_local()
assert symbol.local_id == 5
assert symbol.kind == LocalSymbolKind.Here

expr = BasicExpression('*')
print('BasicExpression:', expr)
print(expr.data)
assert expr.is_valid()
assert expr.is_current_address()
assert not expr.is_number()
assert not expr.is_symbol()
assert expr.kind == BasicExpression.Kind.CurrentAddress
print(expr.as_current_address())

expr = BasicExpression('99999')
print('BasicExpression:', expr)
print(expr.data)
assert expr.is_valid()
assert not expr.is_current_address()
assert expr.is_number()
assert not expr.is_symbol()
assert expr.kind == BasicExpression.Kind.Number
print(expr.as_number().data)

token = ExpressionToken(BasicExpression('42'))
print(token.unary_op)
print(token.basic_expr.data)
print(token.binary_op)

token = ExpressionToken('-', BasicExpression('33'))
print(token.unary_op)
print(token.basic_expr.data)
print(token.binary_op)

token = ExpressionToken(BasicExpression('1'), '*')
print(token.unary_op)
print(token.basic_expr.data)
print(token.binary_op)

token = ExpressionToken('+', BasicExpression('2'), '/')
print(token.unary_op)
print(token.basic_expr.data)
print(token.binary_op)

expr = Expression([
	ExpressionToken(BasicExpression('1'), '+'),
	ExpressionToken(BasicExpression('2'))])
print('Expression:', expr)

for token in expr.tokens:
	print(token)

