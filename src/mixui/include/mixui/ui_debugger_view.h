#pragma once
#include <imgui.h>

namespace mix
{
    class Computer;
} // namespace mix

struct Debugger;

struct UIDebuggerView
{
    float drag_dy_ = 0.f;
    ImVec2 drag_shadow_pos_ = ImVec2(-1.f, -1.f);
    ImVec2 drag_pos_ = ImVec2(-1.f, -1.f);

    int new_address_ = -1;
    int breakpoint_to_add_ = -1;
    int breakpoint_to_remove_ = -1;

    bool is_dragging_active() const;
    void start_dragging(const ImVec2& item_pos
        , float mouse_y = ImGui::GetIO().MousePos.y);
    void stop_dragging();
    bool update_dragging(float mouse_y = ImGui::GetIO().MousePos.y);
};

void UIDebuggerViewInput(const mix::Computer& mix
    , const Debugger& debugger
    , UIDebuggerView* state);
