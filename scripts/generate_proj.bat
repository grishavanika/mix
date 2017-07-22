@echo off
set cmake=C:\Programs\CMake\bin
set mingw=C:\Programs\mingw\bin

:: Build folder relative to `scripts` folder
set build_root=..\build
:: Folder for root CMakeLists.txt relative to build folder below
set source_root=..
set generator=%1

:: Inject our executables
set path=%path%;%cmake%;%mingw%;

call:set_cmake_generator_update_build_dir

if NOT exist %build_root%\NUL (
	mkdir %build_root%
	)

pushd %build_root%

cmake -G %generator% -DCMAKE_INSTALL_PREFIX=deploy %source_root%

popd

goto:eof

:set_cmake_generator_update_build_dir
	if "%generator%" == "" (
		set generator=msvc
	)

	if "%generator%" == "clang" (
		set generator="Visual Studio 15 2017 Win64" -T LLVM-vs2014
		set build_root=%build_root%_clang
	) else if "%generator%" == "gcc" (
		set generator="MinGW Makefiles"
		set build_root=%build_root%_gcc
	) else if "%generator%" == "msvc" (
		set generator="Visual Studio 15 2017 Win64"
		set build_root=%build_root%_msvc
	)
goto:eof
