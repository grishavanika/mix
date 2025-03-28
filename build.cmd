cmake -S . -B build ^
	-DCMAKE_INSTALL_PREFIX=install ^
	-DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake
cmake --build build --config Release
