#define _CRT_SECURE_NO_WARNINGS 1

#include <mix/computer.h>
#include <mix/command.h>
#include <mix/command_processor.h>
#include <mix/default_device.h>
#include <mixal/program_executor.h>
#include <mixal_parse/types/operation_id.h>
#include <mixal/operation_info.h>
#include <mixui/command_help.h>
#include <mixui/ui_word.h>
#include <imgui_stdlib.h>

#include <vector>
#include <algorithm>

#include <cstdio>

#include <fstream>
#include <iomanip>

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>

#include <SDL.h>

#include <GL/gl3w.h>

#include <cassert>
#include <cstdlib>

static void SDLCheck(int status)
{
    if (status != 0)
    {
        assert(false && "SDL2 call failed");
        std::abort();
    }
}

static void GLCheck(int status)
{
    if (status != 0)
    {
        assert(false && "OpenGL call failed");
        std::abort();
    }
}

static void ImGuiCheck(bool status)
{
    if (!status)
    {
        assert(false && "ImGui call failed");
        std::abort();
    }
}

struct WordWithSource
{
    mixal::TranslatedWord translated;
    int line_id = -1;
    mixal::OperationId operation_id = mixal::OperationId::Unknown;
    std::string line;
};

struct ProgramWithSource
{
    std::vector<WordWithSource> commands;
    int start_address = -1;
};

static void LoadProgram(mix::Computer& computer, const ProgramWithSource& program)
{
    for (const auto& word : program.commands)
    {
        const int address = word.translated.original_address;
        if (address >= 0)
        {
            computer.set_memory(
                word.translated.original_address
                , word.translated.value);
        }
    }
    computer.set_next_address(program.start_address);
}

static ProgramWithSource LoadProgramFromSourceFile(
    const std::string& content, mix::Computer& mix)
{
    std::istringstream in(content);
    mixal::Translator translator;
    mixal::LinesTranslator lines_translator(translator);
    std::vector<mixal::TranslatedLine> lines;
    std::vector<std::string> strs;

    try
    {
        std::string str;
        while (getline(in, str))
        {
            lines.push_back(lines_translator.translate(str));
            strs.push_back(std::move(str));
            if (lines.back().end_code)
            {
                break;
            }
        }
    }
    catch (const std::exception&)
    {
        return ProgramWithSource();
    }

    ProgramWithSource program;

    for (int line_id = 0, count = static_cast<int>(lines.size()); line_id < count; ++line_id)
    {
        auto word_ref = lines[line_id].word_ref;
        auto end_code = lines[line_id].end_code;
        auto id = lines[line_id].operation_id;
        auto str = strs[line_id];
        if (word_ref)
        {
            assert(word_ref->is_ready());
            WordWithSource word;
            word.translated = word_ref->translated_word();
            word.line_id = line_id + 1;
            word.operation_id = id;
            word.line = str;
            program.commands.push_back(word);
        }
        else if (end_code)
        {
            program.start_address = end_code->start_address;
            for (auto& end_symbols : end_code->defined_symbols)
            {
                WordWithSource word;
                word.translated = end_symbols.second;
                word.line_id = line_id + 1;
                word.operation_id = id;
                word.line = str;
                program.commands.push_back(word);
            }
        }
        else
        {
            WordWithSource word;
            word.line_id = line_id + 1;
            word.operation_id = id;
            word.line = str;
            program.commands.push_back(word);
        }
    }

    LoadProgram(mix, program);
    return program;
}

struct UIFlags
{
    int comparison_ = 0;
    bool overflow_ = false;

    static int FromComparisonIndicator(mix::ComparisonIndicator state)
    {
        switch (state)
        {
        case mix::ComparisonIndicator::Less   : return 1;
        case mix::ComparisonIndicator::Greater: return 2;
        case mix::ComparisonIndicator::Equal  : return 0;
        }
        return 0;
    }

    mix::ComparisonIndicator to_comparison_indicator() const
    {
        switch (comparison_)
        {
        case 1: return mix::ComparisonIndicator::Less;
        case 2: return mix::ComparisonIndicator::Greater;
        case 0: return mix::ComparisonIndicator::Equal;
        }
        return mix::ComparisonIndicator::Less;
    }

    mix::OverflowFlag to_overflow_flag() const
    {
        return overflow_
            ? mix::OverflowFlag::Overflow
            : mix::OverflowFlag::NoOverflow;
    }

    void set(mix::ComparisonIndicator state, mix::OverflowFlag overflow)
    {
        comparison_ = FromComparisonIndicator(state);
        overflow_ = (overflow == mix::OverflowFlag::Overflow);
    }
};


struct DebuggerState
{
    using Breakpoints = std::vector<int>;

    ProgramWithSource program_;

    std::stringstream device18_;
    Breakpoints breakpoints_;
    int executed_instructions_count = 0;

    bool has_breakpoint(int address) const
    {
        const auto it = std::find(
            std::cbegin(breakpoints_)
            , std::cend(breakpoints_)
            , address);
        return (it != std::cend(breakpoints_));
    }

    void add_breakpoint(int address)
    {
        assert(address >= 0);
        breakpoints_.push_back(address);
    }

    void remove_breakpoint(int address)
    {
        assert(address >= 0);
        const auto it = std::find(
            std::begin(breakpoints_)
            , std::end(breakpoints_)
            , address);
        if (it != std::end(breakpoints_))
        {
            breakpoints_.erase(it);
        }
    }
};

struct UIMix
{
    static constexpr int k_ri_count =
        static_cast<int>(mix::Computer::k_index_registers_count);

    UIMix(mix::Computer& mix, DebuggerState& debugger)
        : mix_(mix)
        , debugger_(debugger)
    {
    }

    mix::Computer& mix_;
    DebuggerState& debugger_;
    UIWord ra_;
    UIWord rx_;
    UIWord rj_; // non-negative, bytes [4, 5]
    UIWord ri_[k_ri_count]; // bytes [4, 5]
    int address_ = 0;
    UIFlags flags_;

    std::string output_;
    std::string source_file_ = R"(C:\dev\mix\src\tests\mixal_code\program_maximum.mixal)";
};

static void SetupIODevice(UIMix& ui_mix)
{
    ui_mix.mix_ = mix::Computer();
    ui_mix.debugger_.device18_.str(std::string());
    ui_mix.mix_.replace_device(18
        , std::make_unique<mix::SymbolDevice>(
            24/*block size*/,
            ui_mix.debugger_.device18_,
            ui_mix.debugger_.device18_,
            false));
}

// #XXX: hard-code bytes formatting to 18 symbols.
// UI should be changed to really render bytes
// instead converting to string & render string
static std::string k_no_code(18, ' ');

static std::string PrintCode(const mix::Word& w, mixal::OperationId operation_id)
{
    std::ostringstream out_;

    const auto prev_fill = out_.fill('0');

    switch (operation_id)
    {
    case mixal::OperationId::CON:
    case mixal::OperationId::END:
        out_.fill(' ');
        out_<< '|' << w.sign() << '|' << std::setw(14)
            << std::right << w.abs_value() << '|';
        break;
    case mixal::OperationId::ALF:
        out_ << w;
        break;
    default:
        out_ << mix::Command(w);
        break;
    }

    out_.fill(prev_fill);
    std::string str = std::move(out_).str();
    assert(str.size() == k_no_code.size());
    return str;
}

static void RegistersInputWindow(UIMix& ui_mix)
{
    if (!ImGui::Begin("Registers"))
    {
        ImGui::End();
        return;
    }
    // ImGui::Separator();
    ImGui::Columns(3, nullptr, true);
    ImGui::PushItemWidth(150);
    if (UIRegisterInput("A", ui_mix.ra_))
    {
        ui_mix.mix_.set_ra(mix::Register(ui_mix.ra_.get()));
    }
    if (UIRegisterInput("X", ui_mix.rx_))
    {
        ui_mix.mix_.set_rx(mix::Register(ui_mix.rx_.get()));
    }
    if (UIAddressRegisterInput("J", ui_mix.rj_))
    {
        ui_mix.mix_.jump(ui_mix.rj_.get().value());
    }
    ImGui::NextColumn();

    auto handle_ri = [&ui_mix](int i)
    {
        char name[32]{};
        (void)snprintf(name, sizeof(name), "I%i", i);
        if (UIIndexRegisterInput(name, ui_mix.ri_[i - 1]))
        {
            const auto word = ui_mix.ri_[i - 1].get();
            ui_mix.mix_.set_ri(i, mix::IndexRegister(word));
        }
    };

    for (int i = 1; i <= 3; ++i)
    {
        handle_ri(i);
    }
    ImGui::NextColumn();

    for (int i = 4; i <= UIMix::k_ri_count; ++i)
    {
        handle_ri(i);
    }

    ImGui::PopItemWidth();
    // ImGui::Separator();
    
    ImGui::End();
}

static bool UIFlagsInput(UIFlags& state)
{
    ImGui::BeginGroup();
    ImGui::PushID("");

    bool changed = false;
    changed |= ImGui::RadioButton("Less", &state.comparison_
        , UIFlags::FromComparisonIndicator(mix::ComparisonIndicator::Less));
    ImGui::SameLine();
    changed |= ImGui::RadioButton("Equal", &state.comparison_
        , UIFlags::FromComparisonIndicator(mix::ComparisonIndicator::Equal));
    ImGui::SameLine();
    changed |= ImGui::RadioButton("Greater", &state.comparison_
        , UIFlags::FromComparisonIndicator(mix::ComparisonIndicator::Greater));
    ImGui::SameLine();
    changed |= ImGui::Checkbox("Overflow", &state.overflow_);

    ImGui::PopID();
    ImGui::EndGroup();

    return changed;
}

static bool UIAddressInput(const char* title, int& address)
{
    const int old_address = address;
    if (ImGui::InputInt(title, &address, 0))
    {
        address = std::clamp(address, 0
            , static_cast<int>(mix::Computer::k_memory_words_count));
        return (old_address != address);
    }
    return false;
}

static void UIMenuInput(UIMix& ui_mix)
{
    bool open_file = false;

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Open"))
            {
                open_file = true;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    // #XXX: temporary load at start our hard-coded source file
    if (open_file || ui_mix.debugger_.program_.commands.empty())
    {
        std::ostringstream ss;
        std::ifstream in(ui_mix.source_file_);
        ss << in.rdbuf();
        const std::string source = std::move(ss).str();

        SetupIODevice(ui_mix);
        ui_mix.debugger_.program_ = LoadProgramFromSourceFile(source, ui_mix.mix_);
        ui_mix.debugger_.breakpoints_.clear();
        ui_mix.debugger_.executed_instructions_count = 0;
    }
}

static void UpdateUIFromMixState(UIMix& ui, const mix::Computer& mix)
{
    ui.address_ = mix.current_address();
    ui.flags_.set(mix.comparison_state(), mix.overflow_flag());
    ui.ra_.set(mix.ra());
    ui.rx_.set(mix.rx());
    ui.rj_.set(mix.rj());
    for (std::size_t i = 1; i <= mix::Computer::k_index_registers_count; ++i)
    {
        ui.ri_[i - 1].set(mix.ri(i));
    }
}

static int GetDigitsCount(int n)
{
    return static_cast<int>(std::floor(std::log10(n) + 1));
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

struct DebuggerUIState
{
    float drag_dy_ = 0.f;
    ImVec2 drag_shadow_pos_ = ImVec2(-1.f, -1.f);
    ImVec2 drag_pos_ = ImVec2(-1.f, -1.f);

    int new_address_ = -1;
    int breakpoint_to_add_ = -1;
    int breakpoint_to_remove_ = -1;

    bool is_dragging_active() const
    {
        return (drag_pos_.y >= 0);
    }

    void start_dragging(const ImVec2& item_pos
        , float mouse_y = ImGui::GetIO().MousePos.y)
    {
        drag_dy_ = mouse_y - item_pos.y;
        drag_pos_ = item_pos;
        drag_shadow_pos_ = item_pos;
    }

    void stop_dragging()
    {
        drag_dy_ = 0.f;
        drag_pos_ = ImVec2(-1.f, -1.f);
        drag_shadow_pos_ = ImVec2(-1.f, -1.f);
    }

    bool update_dragging(float mouse_y = ImGui::GetIO().MousePos.y)
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
};

static void RenderLineByLine(UIMix& ui_mix, int current_address, DebuggerUIState& state)
{
    ImGui::BeginChild("##scrolling", ImVec2(0, -1.f), false/*border*/, ImGuiWindowFlags_NoMove);

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

    const float line_height = ImGui::GetTextLineHeight();
    const int total_lines = static_cast<int>(ui_mix.debugger_.program_.commands.size());
    const int line_number_width = GetDigitsCount(total_lines);
    const int address_width = GetDigitsCount(
        static_cast<int>(mix::Computer::k_memory_words_count));

    state.new_address_ = -1;
    state.breakpoint_to_add_ = -1;
    state.breakpoint_to_remove_ = -1;

#if (0)
    const auto pos = ImGui::GetCursorScreenPos();
    const float GlyphWidth = ImGui::CalcTextSize("0").x + 1;
    draw_list.AddLine(
          ImVec2(pos.x + line_number_width + 2 * GlyphWidth, pos.y)
        , ImVec2(pos.x + line_number_width + 2 * GlyphWidth, pos.y + 9999)
        , ImGui::GetColorU32(ImGuiCol_Border));
#endif

    const bool was_dragging_stopped = state.update_dragging();

    ImGuiListClipper clipper(total_lines, line_height);
    const ImVec2 action_size(line_height, line_height);
    for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i)
    {
        ImGui::PushID(i);

        const WordWithSource& word = ui_mix.debugger_.program_.commands[i];
        const int address = word.translated.original_address;
        std::string line = word.line;

        const ImVec2 pos = ImGui::GetCursorScreenPos();
        // Draw hidden button for "actions" column.
        const bool on_action_press = ImGui::InvisibleButton("##action", action_size);
        const ImVec2 action_rect_min = ImGui::GetItemRectMin();
        const ImVec2 action_rect_max = ImGui::GetItemRectMax();
        const bool is_action_hovered = ImGui::IsItemHovered();

        // Toggle (add/remove) breakpoint
        const bool is_on_breakpoint = ui_mix.debugger_.has_breakpoint(address);
        // To disable breakpoints add/remove when
        // we drop dragged cursor
        if (!was_dragging_stopped
            && on_action_press
            && (address >= 0))
        {
            int& bp = is_on_breakpoint
                ? state.breakpoint_to_remove_
                : state.breakpoint_to_add_;
            bp = address;
        }

        if (is_on_breakpoint)
        {
            RenderBreakpoint(pos, action_size);
        }

        const bool is_active_address = (address >= 0)
            && (current_address == address);
        if (is_active_address && !state.is_dragging_active())
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
            ImGui::Text("% *c", address_width, ' ');
        }
        else
        {
            ImGui::Text("%0*i", address_width, address);
        }
        ImGui::SameLine();

        if (address >= 0)
        {
            const mix::Word memory = ui_mix.mix_.memory(address);
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
            && !state.is_dragging_active()
            && ImGui::GetIO().MouseDown[0])
        {
            state.start_dragging(action_rect_min);
        }
        if (state.is_dragging_active()
            && (address >= 0)
            && !is_active_address)
        {
            const float y_min = action_rect_min.y;
            const float y_max = action_rect_max.y;
            const float drag_y = state.drag_pos_.y + (line_height / 2);
            if ((drag_y >= y_min) && (drag_y <= y_max))
            {
                state.new_address_ = address;
            }
        }
    }

    if (state.is_dragging_active())
    {
        RenderAddressCursor(state.drag_shadow_pos_, action_size, false/*not filled*/);
        RenderAddressCursor(state.drag_pos_, action_size);
    }

    clipper.End();
    ImGui::PopStyleVar(2);
    ImGui::EndChild();
}

static void RenderAll()
{
    // ImGui::ShowDemoWindow(nullptr);

    static mix::Computer mix;
    static mix::CommandProcessor mix_processor(mix);
    static mix::CommandHelp mix_help(mix_processor);
    static DebuggerState debugger;
    static UIMix ui_mix(mix, debugger);

    UpdateUIFromMixState(ui_mix, mix);

    RegistersInputWindow(ui_mix);

    if (ImGui::Begin("Rest"))
    {
        if (UIFlagsInput(ui_mix.flags_))
        {
            ui_mix.mix_.set_comparison_state(
                ui_mix.flags_.to_comparison_indicator());
            ui_mix.mix_.set_overflow_flag(
                ui_mix.flags_.to_overflow_flag());
        }
        if (UIAddressInput("Address", ui_mix.address_))
        {
            ui_mix.mix_.set_next_address(ui_mix.address_);
        }
    }
    ImGui::End();

    if (ImGui::Begin("Help"))
    {
        const std::string help = mix_help.describe_address(
            mix.current_address());
        ImGui::PushTextWrapPos(0);
        ImGui::TextUnformatted(help.c_str()
            , help.c_str() + help.size());
        ImGui::PopTextWrapPos();
    }
    ImGui::End();

    bool run_one = false;
    bool run_to_breakpoint = false;
    if (ImGui::Begin("Editor", nullptr, ImGuiWindowFlags_MenuBar))
    {
        UIMenuInput(ui_mix);

        (void)ImGui::InputText("Source file", &ui_mix.source_file_);

        ImGui::Text("Executed instructions: %i."
            , ui_mix.debugger_.executed_instructions_count);
        ImGui::SameLine();
        ImGui::Text("Status: %s.", ui_mix.mix_.is_halted() ? "Halted" : "Running");

        if (ImGui::Button("Step"))
        {
            run_one = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Run"))
        {
            run_to_breakpoint = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Clear breakpoints"))
        {
            ui_mix.debugger_.breakpoints_.clear();
        }
    }

    int instructions = 0;
    if (run_one)
    {
        instructions += mix.run_one();
    }
    else if (run_to_breakpoint)
    {
        if (ui_mix.debugger_.breakpoints_.empty())
        {
            // No breakpoints, just run to the end
            instructions += mix.run();
        }
        else
        {
            // Run until breakpoint or end
            do
            {
                instructions += mix.run_one();
            }
            while (!mix.is_halted()
                && !debugger.has_breakpoint(mix.current_address()));
        }
    }
    ui_mix.debugger_.executed_instructions_count += instructions;

    if (ImGui::Begin("LineByLine"))
    {
        static DebuggerUIState state;
        RenderLineByLine(ui_mix, mix.current_address(), state);
        if (state.new_address_ >= 0)
        {
            ui_mix.mix_.set_next_address(state.new_address_);
        }
        if (state.breakpoint_to_add_ >= 0)
        {
            ui_mix.debugger_.add_breakpoint(state.breakpoint_to_add_);
        }
        if (state.breakpoint_to_remove_ >= 0)
        {
            ui_mix.debugger_.remove_breakpoint(state.breakpoint_to_remove_);
        }
    }
    ImGui::End();

    ui_mix.output_ = ui_mix.debugger_.device18_.str();
    if (!ui_mix.output_.empty())
    {
        if (ImGui::Begin("Output"))
        {
            (void)ImGui::InputTextMultiline("##Output", &ui_mix.output_
                , ImVec2(-1.f, -1.f), ImGuiInputTextFlags_ReadOnly);
        }
        ImGui::End();
    }

    ImGui::End();
}

#if defined(_WIN32)
#include <Windows.h>
#include <tchar.h>
int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
#else
int main(int, char**)
#endif
{
    SDLCheck(::SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER));

    // Decide GL+GLSL versions
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    SDLCheck(::SDL_GL_SetAttribute(
        // Disable GL "debug" mode
        SDL_GL_CONTEXT_FLAGS, 0));
    SDLCheck(::SDL_GL_SetAttribute(
        // OpenGL core profile - deprecated functions are disabled
        SDL_GL_CONTEXT_PROFILE_MASK
        , SDL_GL_CONTEXT_PROFILE_CORE));
    SDLCheck(::SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3));
    SDLCheck(::SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0));

    // Create window with graphics context
    SDLCheck(::SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1));
    SDLCheck(::SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24));
    SDLCheck(::SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8));
    SDL_Window* window = ::SDL_CreateWindow("MIX UI"
        , SDL_WINDOWPOS_CENTERED
        , SDL_WINDOWPOS_CENTERED
        , 1280, 720
        , SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_GLContext gl_context = ::SDL_GL_CreateContext(window);
    SDLCheck(::SDL_GL_MakeCurrent(window, gl_context));
    GLCheck(::SDL_GL_SetSwapInterval(1)); // Enable vsync

    // Initialize OpenGL loader
    GLCheck(gl3wInit());

    // Setup Dear ImGui context
    (void)IMGUI_CHECKVERSION();
    (void)ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Setup Dear ImGui style
    ImGui::StyleColorsLight();

    // Setup Platform/Renderer bindings
    ImGuiCheck(ImGui_ImplSDL2_InitForOpenGL(window, gl_context));
    ImGuiCheck(ImGui_ImplOpenGL3_Init(glsl_version));

    auto handle_events = [window]
    {
        bool done = false;
        SDL_Event event{};
        while (::SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            done |= (event.type == SDL_QUIT);
            done |= ((event.type == SDL_WINDOWEVENT)
                && (event.window.event == SDL_WINDOWEVENT_CLOSE)
                && (event.window.windowID == ::SDL_GetWindowID(window)));
        }
        return !done;
    };

    // #XXX: add some free, monospace font to resources
    ImFont* default_font = ImGui::GetIO().Fonts->AddFontFromFileTTF(
        R"(C:\Windows\Fonts\consola.ttf)"
        , 18.0);
    if (default_font)
    {
        ImGui::GetIO().Fonts->Build();
        ImGui::GetIO().FontDefault = nullptr;
    }

    auto start_frame = [window, default_font]
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();
    };

    auto end_frame = [window]
    {
        ImGuiIO& io = ImGui::GetIO();
        ImGui::Render();
        glViewport(0, 0
            , static_cast<int>(io.DisplaySize.x)
            , static_cast<int>(io.DisplaySize.y));
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    };

    while (handle_events())
    {
        start_frame();
        RenderAll();
        end_frame();
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
