# ImGui integration

include(sdl2/sdl2_integration.cmake)
include(gl3w/gl3w_integration.cmake)

add_subdirectory(imgui)

set(imgui_dir ${CMAKE_BINARY_DIR}/imgui-src)

add_library(ImGui_Core
	${imgui_dir}/imconfig.h
	${imgui_dir}/imgui.h
	${imgui_dir}/imgui_internal.h
	${imgui_dir}/imgui.cpp
	${imgui_dir}/imgui_demo.cpp
	${imgui_dir}/imgui_draw.cpp
	${imgui_dir}/imgui_widgets.cpp)

target_include_directories(ImGui_Core PUBLIC ${imgui_dir})

add_library(ImGui_Cpp
	${imgui_dir}/misc/cpp/imgui_stdlib.h
	${imgui_dir}/misc/cpp/imgui_stdlib.cpp)

target_include_directories(ImGui_Cpp PUBLIC ${imgui_dir}/misc/cpp)
target_link_libraries(ImGui_Cpp PRIVATE ImGui_Core)

# sdl/opengl3/GL3W
add_library(ImGui_Impl
	${imgui_dir}/examples/imgui_impl_sdl.h
	${imgui_dir}/examples/imgui_impl_opengl3.h
	${imgui_dir}/examples/imgui_impl_sdl.cpp
	${imgui_dir}/examples/imgui_impl_opengl3.cpp)

target_compile_definitions(ImGui_Impl PRIVATE IMGUI_IMPL_OPENGL_LOADER_GL3W)
target_include_directories(ImGui_Impl PUBLIC ${imgui_dir}/examples)
target_link_libraries(ImGui_Impl PRIVATE ImGui_Core)
target_link_libraries(ImGui_Impl PUBLIC SDL2_Integrated gl3w_Integrated)

add_library(ImGui_Integrated INTERFACE)
target_link_libraries(ImGui_Integrated INTERFACE
	ImGui_Core
	ImGui_Impl
	ImGui_Cpp)

set_target_properties(ImGui_Core PROPERTIES FOLDER third_party)
set_target_properties(ImGui_Impl PROPERTIES FOLDER third_party)

