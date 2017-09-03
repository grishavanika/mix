from py_mixal_parse import *

number = Number("42")
print(number)
print(number.data)
assert number.value == 42
assert not number.is_zero()

symbol = Symbol.from_string("5H")
print(symbol)
print(symbol.name)
assert symbol.is_valid()
assert symbol.is_local()
assert symbol.local_id == 5
assert symbol.kind == LocalSymbolKind.Here

expr = BasicExpression("*")
print(expr)
print(expr.data)
assert expr.is_valid()
assert expr.is_current_address()
assert not expr.is_number()
assert not expr.is_symbol()
assert expr.kind == BasicExpression.Kind.CurrentAddress
print(expr.as_current_address())

expr = BasicExpression("99999")
print(expr)
print(expr.data)
assert expr.is_valid()
assert not expr.is_current_address()
assert expr.is_number()
assert not expr.is_symbol()
assert expr.kind == BasicExpression.Kind.Number
print(expr.as_number().data)

# token = ExpressionToken()
# print(token.unary_op)
# print(token.basic_expr)
# print(token.binary_op)


