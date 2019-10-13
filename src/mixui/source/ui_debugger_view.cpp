#include "mixui/ui_debugger_view.h"
#include "mixui/debugger.h"
#include "mixui/utils.h"

#include <mixal/operation_info.h>

#include <mix/computer.h>
#include <mix/command.h>

#include <iomanip>

bool UIDebuggerView::is_dragging_active() const
{
    return (drag_pos_.y >= 0);
}

void UIDebuggerView::start_dragging(const ImVec2& item_pos
    , float mouse_y /*= ImGui::GetIO().MousePos.y*/)
{
    drag_dy_ = mouse_y - item_pos.y;
    drag_pos_ = item_pos;
    drag_shadow_pos_ = item_pos;
}

void UIDebuggerView::stop_dragging()
{
    drag_dy_ = 0.f;
    drag_pos_ = ImVec2(-1.f, -1.f);
    drag_shadow_pos_ = ImVec2(-1.f, -1.f);
}

bool UIDebuggerView::update_dragging(float mouse_y /*= ImGui::GetIO().MousePos.y*/)
{
    if (!is_dragging_active())
    {
        return false;
    }
    if (ImGui::GetIO().MouseDown[0])
    {
        drag_pos_.y = (mouse_y - drag_dy_);
        return false;
    }

    stop_dragging();
    return true;
}

// #XXX: hard-code bytes formatting to 18 symbols.
// UI should be changed to really render bytes
// instead converting to string & rendering the string.
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wexit-time-destructors"
// declaration requires an exit-time destructor
#endif
static const std::string k_no_code(18, ' ');
#if defined(__clang__)
#pragma clang diagnostic pop
#endif

static std::string PrintCode(const mix::Word& w, mixal::OperationId operation_id)
{
    std::ostringstream o;

    switch (operation_id)
    {
    case mixal::OperationId::CON:
    case mixal::OperationId::END:
        o.fill(' ');
        o << '|' << w.sign() << '|' << std::setw(14)
            << std::right << w.abs_value() << '|';
        break;
    case mixal::OperationId::ALF:
        o << w;
        break;
    default:
        o << mix::Command(w);
        break;
    }

    std::string str = std::move(o).str();
    assert(str.size() == k_no_code.size());
    return str;
}

static void RenderBreakpoint(const ImVec2& pos, const ImVec2& size)
{
    const float radius = ((std::min)(size.x, size.y) / 2.f) - 1;
    ImGui::GetWindowDrawList()->AddCircleFilled(
        ImVec2(pos.x + size.x / 2, pos.y + size.y / 2)
        , radius
        , IM_COL32(255, 0, 0, 255));
}

static void RenderAddressCursor(const ImVec2& pos, const ImVec2& size, bool draw_filled = true)
{
    const ImVec2 rect[2] =
    {
        ImVec2(pos.x, pos.y + size.y / 4 + 1),
        ImVec2(pos.x + size.x / 2, pos.y + (3 * size.y) / 4 - 1)
    };
    const ImVec2 triangle[3] =
    {
        ImVec2(pos.x + size.x / 2 - 1, pos.y + 1),
        ImVec2(pos.x + size.x / 2 - 1, pos.y + size.y - 1),
        ImVec2(pos.x + size.x - 1, pos.y + size.y / 2)
    };
    const ImVec2 lines[] =
    {
        rect[0], ImVec2(rect[0].x, rect[1].y),
        rect[0], ImVec2(rect[1].x, rect[0].y),
        ImVec2(rect[0].x, rect[1].y), rect[1],
        triangle[0], ImVec2(rect[1].x, rect[0].y),
        triangle[0], triangle[2],
        triangle[1], ImVec2(rect[1].x, rect[1].y),
        triangle[1], triangle[2],
    };

    const ImU32 color1 = IM_COL32(255, 216, 55, 255);
    const ImU32 color2 = IM_COL32(94, 94, 94, 255);
    if (draw_filled)
    {
        ImGui::GetWindowDrawList()->AddRectFilled(
              rect[0], rect[1], color1);
        ImGui::GetWindowDrawList()->AddTriangleFilled(
              triangle[0], triangle[1], triangle[2], color1);
    }
    for (int i = 0; i < IM_ARRAYSIZE(lines); i += 2)
    {
        ImGui::GetWindowDrawList()->AddLine(lines[i], lines[i + 1], color2);
    }
}

void UIDebuggerViewInput(const mix::Computer& mix
    , const Debugger& debugger
    , UIDebuggerView* state)
{
    state->new_address_ = -1;
    state->breakpoint_to_add_ = -1;
    state->breakpoint_to_remove_ = -1;

    if (!ImGui::Begin("Debugger"))
    {
        ImGui::End();
        return;
    }

    const int current_address = mix.current_address();
    const int total_lines = static_cast<int>(debugger.program_.commands.size());

    ImGui::BeginChild("##scrolling", ImVec2(0, -1.f), false/*border*/, ImGuiWindowFlags_NoMove);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

    const float line_height = ImGui::GetTextLineHeight();
    const int line_number_width = GetDigitsCount(total_lines);
    const int address_width = GetDigitsCount(
        static_cast<int>(mix::Computer::k_memory_words_count));

#if (0)
    const auto pos = ImGui::GetCursorScreenPos();
    const float GlyphWidth = ImGui::CalcTextSize("0").x + 1;
    draw_list.AddLine(
          ImVec2(pos.x + line_number_width + 2 * GlyphWidth, pos.y)
        , ImVec2(pos.x + line_number_width + 2 * GlyphWidth, pos.y + 9999)
        , ImGui::GetColorU32(ImGuiCol_Border));
#endif

    const bool was_dragging_stopped = state->update_dragging();

    ImGuiListClipper clipper(total_lines, line_height);
    const ImVec2 action_size(line_height, line_height);
    for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i)
    {
        ImGui::PushID(i);

        const WordWithSource& word = debugger.program_.commands[i];
        const int address = word.translated.original_address;
        std::string line = word.line;

        const ImVec2 pos = ImGui::GetCursorScreenPos();
        // Draw hidden button for "actions" column.
        const bool on_action_press = ImGui::InvisibleButton("##action", action_size);
        const ImVec2 action_rect_min = ImGui::GetItemRectMin();
        const ImVec2 action_rect_max = ImGui::GetItemRectMax();
        const bool is_action_hovered = ImGui::IsItemHovered();

        // Toggle (add/remove) breakpoint
        const bool is_on_breakpoint = debugger.has_breakpoint(address);
        // To disable breakpoints add/remove when
        // we drop dragged cursor
        if (!was_dragging_stopped
            && on_action_press
            && (address >= 0))
        {
            int& bp = is_on_breakpoint
                ? state->breakpoint_to_remove_
                : state->breakpoint_to_add_;
            bp = address;
        }

        if (is_on_breakpoint)
        {
            RenderBreakpoint(pos, action_size);
        }

        const bool is_active_address = (address >= 0)
            && (current_address == address);
        if (is_active_address && !state->is_dragging_active())
        {
            RenderAddressCursor(pos, action_size);
        }

        ImGui::SameLine();
        ImGui::Text("%0*i| ", line_number_width, i + 1);
        ImGui::SameLine();

        if (is_active_address)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.f, 0.5f, 0.25f, 1.0f));
        }

        if (address < 0)
        {
            // #TODO: '.' should be ' ' (space)
            ImGui::Text("%.*s", address_width, " ");
        }
        else
        {
            ImGui::Text("%0*i", address_width, address);
        }
        ImGui::SameLine();

        if (address >= 0)
        {
            const mix::Word memory = mix.memory(address);
            const bool modified = (word.translated.value != memory);
            const auto bytecode_str = PrintCode(memory, modified
                ? mixal::QueryOperationInfo(memory).id
                : word.operation_id);
            if (modified)
            {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 0.f, 0.f, 1.0f));
            }
            ImGui::TextUnformatted(bytecode_str.c_str()
                , bytecode_str.c_str() + bytecode_str.size());
            if (modified)
            {
                ImGui::PopStyleColor();
            }
        }
        else
        {
            ImGui::TextUnformatted(k_no_code.c_str()
                , k_no_code.c_str() + k_no_code.size());
        }


        ImGui::SameLine();
        ImGui::Text("    %s", line.c_str());

        if (is_active_address)
        {
            ImGui::PopStyleColor();
        }

        ImGui::PopID();

        // Handle dragging part.
        if (is_active_address
            && is_action_hovered
            && !state->is_dragging_active()
            && ImGui::GetIO().MouseDown[0])
        {
            state->start_dragging(action_rect_min);
        }
        if (state->is_dragging_active()
            && (address >= 0)
            && !is_active_address)
        {
            const float y_min = action_rect_min.y;
            const float y_max = action_rect_max.y;
            const float drag_y = state->drag_pos_.y + (line_height / 2);
            if ((drag_y >= y_min) && (drag_y <= y_max))
            {
                state->new_address_ = address;
            }
        }
    }

    if (state->is_dragging_active())
    {
        RenderAddressCursor(state->drag_shadow_pos_, action_size, false/*not filled*/);
        RenderAddressCursor(state->drag_pos_, action_size);
    }

    clipper.End();
    ImGui::PopStyleVar(2);
    ImGui::EndChild();
    ImGui::End();
}

