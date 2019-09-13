#define _CRT_SECURE_NO_WARNINGS 1

#include <mix/computer.h>
#include <mix/command.h>
#include <mix/default_device.h>
#include <mixal/program_executor.h>
#include <mixal_parse/types/operation_id.h>
#include <mixui/ui_word.h>
#include <imgui_stdlib.h>

#include <vector>
#include <algorithm>

#include <D:\Downloads\imgui_memory_editor.h>

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

    static int ComparisonFrom(mix::ComparisonIndicator state)
    {
        switch (state)
        {
        case mix::ComparisonIndicator::Less: return 1;
        case mix::ComparisonIndicator::Greater: return 2;
        case mix::ComparisonIndicator::Equal: return 0;
        }
        return 0;
    }

    void set(mix::ComparisonIndicator state, mix::OverflowFlag overflow)
    {
        comparison_ = ComparisonFrom(state);
        overflow_ = (overflow == mix::OverflowFlag::Overflow);
    }
};

struct UIMix
{
    static constexpr int k_ri_count =
        static_cast<int>(mix::Computer::k_index_registers_count);

    UIMix(mix::Computer& mix)
        : mix_(mix)
    {
    }

    mix::Computer& mix_;
    UIWord ra_;
    UIWord rx_;
    UIWord rj_; // non-negative, bytes [4, 5]
    UIWord ri_[k_ri_count]; // bytes [4, 5]
    int address_ = 0;
    UIFlags flags_;

    ProgramWithSource program_;

    std::stringstream device0_;
    std::string output_;
    std::vector<int> breakpoints_;

    bool run_one_ = false;
    bool run_to_breakpoint_ = false;
    int executed_instructions_count = 0;

    std::string source_file_ = R"(C:\dev\mix\src\tests\mixal_code\program_primes.mixal)";
};

static void PrepareMix(UIMix& ui_mix)
{
    ui_mix.device0_.str(std::string());
    ui_mix.mix_ = mix::Computer();
    ui_mix.mix_.replace_device(18
        , std::make_unique<mix::SymbolDevice>(
            24/*block size*/,
            ui_mix.device0_,
            ui_mix.device0_,
            false));
}

static void PrintCode(const WordWithSource& word, std::ostream& out_)
{
    if (word.translated.original_address < 0)
    {
        out_ << std::setw(18) << ' ';
        return;
    }

    const auto prev_fill = out_.fill('0');

    switch (word.operation_id)
    {
    case mixal::OperationId::CON:
    case mixal::OperationId::END:
        out_.fill(' ');
        out_<< '|' << word.translated.value.sign() << '|' << std::setw(14)
            << std::right << word.translated.value.abs_value() << '|' << "\n";
        break;
    case mixal::OperationId::ALF:
        out_ << word.translated.value << "\n";
        break;
    default:
        out_ << mix::Command(word.translated.value) << "\n";
        break;
    }

    out_.fill(prev_fill);
}

static std::string PrintCode(const WordWithSource& word)
{
    std::ostringstream out;
    PrintCode(word, out);
    return std::move(out).str();
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
    (void)UIRegisterInput("A", ui_mix.ra_);
    (void)UIRegisterInput("X", ui_mix.rx_);
    (void)UIAddressRegisterInput("J", ui_mix.rj_);
    ImGui::NextColumn();

    auto handle_ri = [&ui_mix](int i)
    {
        char name[32]{};
        (void)snprintf(name, sizeof(name), "I%i", i);
        (void)UIIndexRegisterInput(name, ui_mix.ri_[i - 1]);
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
        , UIFlags::ComparisonFrom(mix::ComparisonIndicator::Less));
    ImGui::SameLine();
    changed |= ImGui::RadioButton("Equal", &state.comparison_
        , UIFlags::ComparisonFrom(mix::ComparisonIndicator::Equal));
    ImGui::SameLine();
    changed |= ImGui::RadioButton("Greater", &state.comparison_
        , UIFlags::ComparisonFrom(mix::ComparisonIndicator::Greater));
    ImGui::SameLine();
    ImGui::Checkbox("Overflow", &state.overflow_);

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

    if (open_file)
    {
        std::ostringstream ss;
        std::ifstream in(ui_mix.source_file_);
        ss << in.rdbuf();
        const std::string source = std::move(ss).str();

        PrepareMix(ui_mix);
        ui_mix.program_ = LoadProgramFromSourceFile(source, ui_mix.mix_);
        ui_mix.breakpoints_.clear();
        ui_mix.executed_instructions_count = 0;
        ui_mix.run_one_ = false;
        ui_mix.run_to_breakpoint_ = false;
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

static void RenderCurrentStep(const ImVec2& pos, const ImVec2& size)
{
    const ImU32 color = IM_COL32(255, 128, 0, 255);
    ImGui::GetWindowDrawList()->AddRectFilled(
          ImVec2(pos.x, pos.y + size.y / 3)
        , ImVec2(pos.x + size.x / 2, pos.y + 2 * (size.y / 3))
        , color);
    ImGui::GetWindowDrawList()->AddTriangleFilled(
          ImVec2(pos.x + size.x / 2, pos.y + 1)
        , ImVec2(pos.x + size.x / 2, pos.y + size.y)
        , ImVec2(pos.x + size.x - 1, pos.y + size.y / 2 - 1)
        , color);
}

static void RenderLineByLine(UIMix& ui_mix, int current_address)
{
    ImGui::BeginChild("##scrolling", ImVec2(0, -1.f), false/*border*/, ImGuiWindowFlags_NoMove);

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

    ImDrawList& draw_list = *ImGui::GetWindowDrawList();
    (void)draw_list;

    const float line_height = ImGui::GetTextLineHeight();
    const int total_lines = static_cast<int>(ui_mix.program_.commands.size());
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

    auto find_breakpoint = [&](int address)
    {
        return std::find(std::begin(ui_mix.breakpoints_)
            , std::end(ui_mix.breakpoints_)
            , address);
    };
    auto is_valid_breakpoint = [&](auto bp_it)
    {
        return (bp_it != std::cend(ui_mix.breakpoints_));
    };

    ImGuiListClipper clipper(total_lines, line_height);
    for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i)
    {
        ImGui::PushID(i);
        const WordWithSource& word = ui_mix.program_.commands[i];
        const int address = word.translated.original_address;
        const std::string& line = word.line;

        const bool is_active_address = (address >= 0)
            && (current_address >= 0)
            && (current_address == address);
        auto breakpoint_it = find_breakpoint(address);

        const ImVec2 pos = ImGui::GetCursorScreenPos();
        const ImVec2 action_size(line_height, line_height);
        if (ImGui::InvisibleButton("##action", action_size))
        {
            if (is_valid_breakpoint(breakpoint_it))
            {
                ui_mix.breakpoints_.erase(breakpoint_it);
                breakpoint_it = ui_mix.breakpoints_.end();
            }
            else if (address >= 0)
            {
                ui_mix.breakpoints_.push_back(address);
                breakpoint_it = (ui_mix.breakpoints_.end() - 1);
            }
        }
        if (is_active_address)
        {
            RenderCurrentStep(pos, action_size);
        }
        else if (is_valid_breakpoint(breakpoint_it))
        {
            RenderBreakpoint(pos, action_size);
        }

        ImGui::SameLine();
        ImGui::Text("%0*i| ", line_number_width, i + 1);
        ImGui::SameLine();

        if (is_active_address)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.f, 0.5f, 0.1f, 1.0f));
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
        const auto bytecode_str = PrintCode(word);
        ImGui::TextUnformatted(bytecode_str.c_str()
            , bytecode_str.c_str() + bytecode_str.size());
        ImGui::SameLine();
        ImGui::Text("    %s", line.c_str());

        if (is_active_address)
        {
            ImGui::PopStyleColor();
        }

        ImGui::PopID();
    }
    clipper.End();
    ImGui::PopStyleVar(2);
    ImGui::EndChild();
}

static void RenderAll()
{
#if (0)
    static MemoryEditor mem_edit_2;
    static char data[1000];
    mem_edit_2.DrawWindow("Test", data, sizeof(data));
    ImGui::ShowDemoWindow(nullptr);
#endif

    static mix::Computer mix;
    static UIMix ui_mix(mix);

    UpdateUIFromMixState(ui_mix, mix);

    RegistersInputWindow(ui_mix);

    if (ImGui::Begin("Rest"))
    {
        (void)UIFlagsInput(ui_mix.flags_);
        (void)UIAddressInput("Address", ui_mix.address_);
    }
    ImGui::End();

    if (ImGui::Begin("Editor", nullptr, ImGuiWindowFlags_MenuBar))
    {
        UIMenuInput(ui_mix);

        (void)ImGui::InputText("Source file", &ui_mix.source_file_);

        ImGui::Text("Executed instructions: %i."
            , ui_mix.executed_instructions_count);
        ImGui::SameLine();
        ImGui::Text("Status: %s.", mix.is_halted() ? "Halted" : "Running");

        if (ImGui::Button("Step"))
        {
            ui_mix.run_one_ = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Run"))
        {
            ui_mix.run_to_breakpoint_ = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Clear breakpoints"))
        {
            ui_mix.breakpoints_.clear();
        }
    }

    auto is_on_breakpoint = [&]()
    {
        const int address = mix.current_address();
        const auto it = std::find(std::cbegin(ui_mix.breakpoints_)
            , std::cend(ui_mix.breakpoints_)
            , address);
        return (it != std::cend(ui_mix.breakpoints_));
    };

    if (ui_mix.run_one_)
    {
        ui_mix.executed_instructions_count += mix.run_one();
        ui_mix.run_one_ = false;
    }
    else if (ui_mix.run_to_breakpoint_)
    {
        ui_mix.run_to_breakpoint_ = false;
        if (ui_mix.breakpoints_.empty())
        {
            ui_mix.executed_instructions_count += mix.run();
        }
        else
        {
            do
            {
                ui_mix.executed_instructions_count += mix.run_one();
            }
            while (!mix.is_halted() && !is_on_breakpoint());
        }
        ui_mix.run_to_breakpoint_ = false;
    }

    if (ImGui::Begin("LineByLine"))
    {
        RenderLineByLine(ui_mix, mix.current_address());
    }
    ImGui::End();

    ui_mix.output_ = ui_mix.device0_.str();
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

    auto start_frame = [window]
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
