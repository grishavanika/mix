include(sdl2_integration.cmake)
find_package(OpenGL REQUIRED)

include(FetchContent)
FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG        7674cbc9b25668dcbcc0ffd35b126b094c978e89
)
FetchContent_MakeAvailable(imgui)

add_library(ImGui_Core
    ${imgui_SOURCE_DIR}/imconfig.h
    ${imgui_SOURCE_DIR}/imgui.h
    ${imgui_SOURCE_DIR}/imgui_internal.h
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp)

target_include_directories(ImGui_Core PUBLIC ${imgui_SOURCE_DIR})
target_include_directories(ImGui_Core PUBLIC ${imgui_SOURCE_DIR}/backends)

add_library(ImGui_Cpp
    ${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.h
    ${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp)

target_include_directories(ImGui_Cpp PUBLIC ${imgui_SOURCE_DIR}/misc/cpp)
target_link_libraries(ImGui_Cpp PRIVATE ImGui_Core)

# sdl/opengl3/SDL2
add_library(ImGui_Impl
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl2.h
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3_loader.h
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.h
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
    )

target_include_directories(ImGui_Impl PUBLIC ${imgui_SOURCE_DIR}/examples)
target_link_libraries(ImGui_Impl PRIVATE ImGui_Core)
target_link_libraries(ImGui_Impl PUBLIC SDL2_Integrated)
target_link_libraries(ImGui_Impl PRIVATE ${OPENGL_gl_LIBRARY})

add_library(ImGui_Integrated INTERFACE)
target_link_libraries(ImGui_Integrated INTERFACE
    ImGui_Core
    ImGui_Impl
    ImGui_Cpp)

set_target_properties(ImGui_Core PROPERTIES FOLDER third_party)
set_target_properties(ImGui_Impl PROPERTIES FOLDER third_party)
set_target_properties(ImGui_Cpp PROPERTIES FOLDER third_party)
