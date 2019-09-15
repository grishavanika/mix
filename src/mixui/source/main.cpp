#define _CRT_SECURE_NO_WARNINGS 1

#include <mix/computer.h>
#include <mix/command.h>
#include <mix/command_processor.h>
#include <mix/default_device.h>
#include <mixal/program_executor.h>
#include <mixal_parse/types/operation_id.h>
#include <mixal/operation_info.h>

#include <imgui.h>
#include <imgui_stdlib.h>

#include <mixui/debugger.h>
#include <mixui/command_help.h>
#include <mixui/ui_debugger_view.h>
#include <mixui/ui_word.h>
#include <mixui/ui_mix_flags.h>

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

//////////////////////////////////////////////// 
static bool UIAddressInput(const char* title, int* address)
{
    const int old_address = *address;
    if (ImGui::InputInt(title, address, 0))
    {
        *address = std::clamp(*address, 0
            , static_cast<int>(mix::Computer::k_memory_words_count) - 1);
        return (old_address != *address);
    }
    return false;
}

struct UIControls
{
    bool run_one_ = false;
    bool run_to_breakpoint_ = false;
    bool load_from_file = false;
    bool clear_breakpoints_ = false;
    std::string source_file_ = R"(C:\dev\mix\src\tests\mixal_code\program_maximum.mixal)";
};

struct UIMix
{
    static constexpr int k_ri_count =
        static_cast<int>(mix::Computer::k_index_registers_count);

    UIWord ra_;
    UIWord rx_;
    UIWord rj_; // non-negative, bytes [4, 5]
    UIWord ri_[k_ri_count]; // bytes [4, 5]
    int address_ = 0;
    
    UIFlags flags_;
    UIControls controls_;
    UIDebuggerView debugger_view_;
};

static void UIControlsInput(UIControls* ui_controls
    , const mix::Computer& mix, const Debugger& debugger)
{
    if (!ImGui::Begin("Editor"))
    {
        ImGui::End();
        return;
    }

    ui_controls->load_from_file = ImGui::Button("Load");
    ImGui::SameLine();
    (void)ImGui::InputText("Source file", &ui_controls->source_file_);
    ui_controls->run_one_ = ImGui::Button("Step");
    ImGui::SameLine();
    ui_controls->run_to_breakpoint_ = ImGui::Button("Run");

    ui_controls->clear_breakpoints_ = false;
    if (!debugger.breakpoints_.empty())
    {
        ImGui::SameLine();
        ui_controls->clear_breakpoints_ = ImGui::Button("Remove all breakpoints");
    }

    ImGui::Text("Executed instructions count: %i.", debugger.executed_instructions_count);
    ImGui::SameLine();
    ImGui::Text("Status: %s.", mix.is_halted() ? "Halted" : "Debugging");

    ImGui::End();
}

static void SetupIODevice(Debugger* debugger, mix::Computer* mix)
{
    *mix = mix::Computer();
    debugger->device18_.str(std::string());
    mix->replace_device(18
        , std::make_unique<mix::SymbolDevice>(
            24/*block size*/,
            debugger->device18_/*output*/,
            debugger->device18_/*input*/,
            false));
}

static void UIControlsModifyMix(UIMix* ui_mix, mix::Computer* mix, Debugger* debugger)
{
    if (ui_mix->controls_.load_from_file)
    {
        SetupIODevice(debugger, mix);
        debugger->program_ = LoadProgramFromSourceFile(ui_mix->controls_.source_file_, mix);
        debugger->loaded_ = true;
        debugger->breakpoints_.clear();
        debugger->executed_instructions_count = 0;
    }
    if (ui_mix->controls_.clear_breakpoints_)
    {
        debugger->breakpoints_.clear();
    }
    if (ui_mix->controls_.run_one_)
    {
        debugger->executed_instructions_count += mix->run_one();
    }
    if (ui_mix->controls_.run_to_breakpoint_)
    {
        // Run until breakpoint or end
        do
        {
            debugger->executed_instructions_count += mix->run_one();
        }
        while (!mix->is_halted()
            && !debugger->has_breakpoint(mix->current_address()));
    }
}

static void UIDebuggerModifyMix(const UIDebuggerView& debugger_view
    , Debugger* debugger
    , mix::Computer* mix)
{
    if (debugger_view.new_address_ >= 0)
    {
        mix->set_next_address(debugger_view.new_address_);
    }
    if (debugger_view.breakpoint_to_add_ >= 0)
    {
        debugger->add_breakpoint(debugger_view.breakpoint_to_add_);
    }
    if (debugger_view.breakpoint_to_remove_ >= 0)
    {
        debugger->remove_breakpoint(debugger_view.breakpoint_to_remove_);
    }
}

static void UIRegistersInputModifyMix(UIMix* ui_mix, mix::Computer* mix)
{
    if (!ImGui::Begin("Registers"))
    {
        ImGui::End();
        return;
    }

    ImGui::Columns(3, nullptr, true);
    // #XXX: hard-coded width, ignores style
    ImGui::PushItemWidth(150);

    if (UIRegisterInput("A", ui_mix->ra_))
    {
        mix->set_ra(mix::Register(ui_mix->ra_.get()));
    }
    if (UIRegisterInput("X", ui_mix->rx_))
    {
        mix->set_rx(mix::Register(ui_mix->rx_.get()));
    }
    if (UIAddressRegisterInput("J", ui_mix->rj_))
    {
        mix->jump(ui_mix->rj_.get().value());
    }

    auto handle_ri = [ui_mix, mix](int i)
    {
        char name[32]{};
        (void)snprintf(name, sizeof(name), "I%i", i);
        if (UIIndexRegisterInput(name, ui_mix->ri_[i - 1]))
        {
            const auto word = ui_mix->ri_[i - 1].get();
            mix->set_ri(i, mix::IndexRegister(word));
        }
    };

    ImGui::NextColumn();
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
    ImGui::End();
}

static void UIFlagsAndAddressInputModifyMix(UIMix* ui_mix, mix::Computer* mix)
{
    if (!ImGui::Begin("Rest"))
    {
        ImGui::End();
        return;
    }

    if (UIFlagsInput(ui_mix->flags_))
    {
        mix->set_comparison_state(
            ui_mix->flags_.to_comparison_indicator());
        mix->set_overflow_flag(
            ui_mix->flags_.to_overflow_flag());
    }

    if (UIAddressInput("Address", &ui_mix->address_))
    {
        mix->set_next_address(ui_mix->address_);
    }

    ImGui::End();
}

static void MixModifyUI(const mix::Computer& mix, UIMix* ui)
{
    ui->address_ = mix.current_address();
    ui->flags_.set(mix.comparison_state(), mix.overflow_flag());
    ui->ra_.set(mix.ra());
    ui->rx_.set(mix.rx());
    ui->rj_.set(mix.rj());
    for (std::size_t i = 1; i <= mix::Computer::k_index_registers_count; ++i)
    {
        ui->ri_[i - 1].set(mix.ri(i));
    }
}

void UIRenderOutputAndHelp(const mix::Computer& mix
    , const mix::CommandHelp& mix_help
    , const Debugger& debugger)
{
    if (ImGui::Begin("Output"))
    {
        std::string output = debugger.device18_.str();
        (void)ImGui::InputTextMultiline("##Output", &output
            , ImVec2(-1.f, -1.f), ImGuiInputTextFlags_ReadOnly);
    }
    ImGui::End();

    if (ImGui::Begin("Help"))
    {
        const std::string help = mix_help.describe_address(mix.current_address());
        ImGui::PushTextWrapPos(0);
        ImGui::TextUnformatted(help.c_str()
            , help.c_str() + help.size());
        ImGui::PopTextWrapPos();
    }
    ImGui::End();
}

struct State
{
    mix::Computer mix_;
    mix::CommandProcessor mix_processor_;
    Debugger debugger_;
    UIMix ui_mix_;
    State()
        : mix_()
        , mix_processor_(mix_)
        , debugger_()
        , ui_mix_()
    {
    }
};

static void RenderAll(State* state)
{
#if (0)
    ImGui::ShowDemoWindow(nullptr);
#endif

    MixModifyUI(state->mix_, &state->ui_mix_);
    UIRegistersInputModifyMix(&state->ui_mix_, &state->mix_);
    UIFlagsAndAddressInputModifyMix(&state->ui_mix_, &state->mix_);
    UIControlsInput(&state->ui_mix_.controls_, state->mix_, state->debugger_);
    UIControlsModifyMix(&state->ui_mix_, &state->mix_, &state->debugger_);
    UIDebuggerViewInput(state->mix_, state->debugger_, &state->ui_mix_.debugger_view_);
    UIDebuggerModifyMix(state->ui_mix_.debugger_view_, &state->debugger_, &state->mix_);
    UIRenderOutputAndHelp(state->mix_, mix::CommandHelp(state->mix_processor_), state->debugger_);
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

    State state;

    auto handle_keyboard = [&state](int scancode)
    {
        if (scancode == SDL_SCANCODE_F5)
        {
            if (!state.debugger_.loaded_ || state.mix_.is_halted())
            {
                state.ui_mix_.controls_.load_from_file = true;
            }
            else
            {
                state.ui_mix_.controls_.run_to_breakpoint_ = true;
            }
            UIControlsModifyMix(&state.ui_mix_, &state.mix_, &state.debugger_);
        }
        if (scancode == SDL_SCANCODE_F10)
        {
            if (!state.debugger_.loaded_ || state.mix_.is_halted())
            {
                state.ui_mix_.controls_.load_from_file = true;
            }
            else
            {
                state.ui_mix_.controls_.run_one_ = true;
            }
            UIControlsModifyMix(&state.ui_mix_, &state.mix_, &state.debugger_);
        }
    };

    auto handle_events = [window, &handle_keyboard]
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
            if (event.type == SDL_KEYDOWN)
            {
                // Ignore ImGui::GetIO().WantCaptureKeyboard
                handle_keyboard(event.key.keysym.scancode);
            }
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
        RenderAll(&state);
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
