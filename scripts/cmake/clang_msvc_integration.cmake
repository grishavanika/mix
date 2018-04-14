
macro(make_clang_work_on_windows)

	add_compile_options(/EHa)

	#INV: why this happens with Clang toolset ?
	add_definitions(-D_HAS_CXX17=1)
	set(msvc_version "14.13.26128")
    set(msvc_install_root "C:/Programs")
    include_directories(SYSTEM "${msvc_install_root}/Microsoft Visual Studio/2017/Community/VC/Tools/MSVC/${msvc_version}/include")

	# MSVC warnings in system headers
	add_compile_options(
		-Wno-deprecated-declarations
		-Wno-unknown-argument
		-Wno-unused-command-line-argument
		-Wno-ignored-attributes
		-Wno-microsoft-template
		-Wno-dllimport-static-field-def
		-Wno-dangling-else
		-Wno-logical-op-parentheses
		-Wno-unused-local-typedef
		-Wno-invalid-noreturn
		-Wno-reserved-id-macro
		-Wno-c++98-compat
		-Wno-c++98-compat-pedantic
		-Wno-language-extension-token
		-Wno-old-style-cast
		-Wno-deprecated-dynamic-exception-spec
		-Wno-zero-as-null-pointer-constant
		-Wno-double-promotion
		-Wno-comma
		-Wno-sign-conversion
		-Wno-c++17-extensions
		-Wno-implicit-fallthrough
		-Wno-global-constructors
		-Wno-shadow-uncaptured-local
		-Wno-cast-qual
		-Wno-undefined-reinterpret-cast
		-Wno-float-equal
		-Wno-shadow-field)

endmacro()
