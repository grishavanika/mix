@echo off
set generator=%1
set config=%2

call install_py_bindings.bat %generator% %config%

python ..\src\py_bindings\tests\test_mixal_parse.py

