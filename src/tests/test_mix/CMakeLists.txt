cmake_minimum_required(VERSION 3.9.1)

# set PCH for VS project
# https://stackoverflow.com/questions/148570/using-pre-compiled-headers-with-cmake
function(set_target_precompiled_header target pc_header pc_source)
  if (only_msvc)
     set_target_properties(${target} PROPERTIES COMPILE_FLAGS "/Yu${pc_header}")
     set_source_files_properties(${pc_source} PROPERTIES COMPILE_FLAGS "/Yc${pc_header}")
  endif ()
endfunction(set_target_precompiled_header)

set(exe_name test_mix)

file(GLOB_RECURSE ${exe_name}_src "source" "*.cpp")
file(GLOB_RECURSE ${exe_name}_hds "source" "*.h")

list(APPEND ${exe_name}_src precompiled.cpp)
list(APPEND ${exe_name}_hds precompiled.h)

add_executable(${exe_name} ${${exe_name}_src} ${${exe_name}_hds})
target_include_directories(${exe_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

set_target_precompiled_header(${exe_name} precompiled.h precompiled.cpp)

target_link_libraries(${exe_name} PRIVATE mix_lib)
target_link_libraries(${exe_name} PRIVATE gtest gmock gmock_main)

install(TARGETS ${exe_name}
	DESTINATION bin/Debug/tests
	CONFIGURATIONS Debug)

install(TARGETS ${exe_name}
	DESTINATION bin/Release/tests
	CONFIGURATIONS Release)
