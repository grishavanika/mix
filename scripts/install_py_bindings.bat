@echo off
set generator=%1
set config=%2

call build.bat %generator% %config%

set pip_path=%python%\Scripts
set path=%path%;%pip_path%

set pyds_root=%build_root%/deploy/bin/%config%/py

:: Workaround for passing options to setup.py
:: (absolute path to folter where all .pyd files are put)
set pyds_root=%cd%/%pyds_root%
pip install . --upgrade

