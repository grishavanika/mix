set(exe_name test_mix)

target_collect_sources(${exe_name})

add_executable(${exe_name} ${${exe_name}_files})
target_include_directories(${exe_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

set_all_warnings(${exe_name} PRIVATE)

set_target_precompiled_header(${exe_name} precompiled.h precompiled.cpp)

target_link_libraries(${exe_name} PRIVATE mix_lib)
target_link_libraries(${exe_name} PRIVATE GTest_Integrated)

target_install_binaries(${exe_name})

if (clang_on_msvc)
	# Mocks
	target_compile_options(${exe_name} PRIVATE
		-Wno-non-virtual-dtor)
endif()

if (BUILD_SHARED_LIBS)
	target_compile_options(${exe_name} PRIVATE
		-DGTEST_LINKED_AS_SHARED_LIBRARY=1)
endif()

