@echo off
:: Note: should be configured by every user
set cmake=C:\Programs\CMake\bin
set mingw=C:\Programs\mingw\bin

set build_root=..\build
set name=note
set generator=%1
set config=%2

:: Inject our executables
set path=%path%;%cmake%;%mingw%;

call:update_build_dir

:: Generate files if it was not done before.
:: Note: this will not stop execution in case of error(s)
if NOT exist %build_root%\NUL (
	call generate_proj.bat %generator%
	)

if "%config%" == "" (
	set config=Debug
	)

pushd %build_root%

:: Build all
cmake --build . --config %config%
if NOT %errorlevel% == 0 (
	goto:eof
	)

:: Install all
:: cmake --build . --config %config% --target install 
:: Workaround for broken CMake's Makefile install (?)
cmake -DBUILD_TYPE=%config% -P cmake_install.cmake

:: Check with tests
ctest -C %config% --verbose
if NOT %errorlevel% == 0 (
	goto:eof
	)

popd

:: End
goto:eof

:update_build_dir
	if "%generator%" == "" (
		set generator=msvc
	)

	if "%generator%" == "clang" (
		set build_root=%build_root%_clang
	) else if "%generator%" == "gcc" (
		set build_root=%build_root%_gcc
	) else if "%generator%" == "msvc" (
		set build_root=%build_root%_msvc
	)
goto:eof
