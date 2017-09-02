from py_mixal_parse import *

expr = BasicExpression("*")
assert expr.is_current_address()
print(expr.kind)

expr = BasicExpression("99999")
assert expr.is_number()
print(expr.as_number().data)

