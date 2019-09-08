#include <imgui.h>
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

#include <mix/computer.h>
#include <mixui/ui_word.h>

#include <cstdio>

struct UIMix
{
    static constexpr int k_ri_count =
        static_cast<int>(mix::Computer::k_index_registers_count);

    UIWord ra_;
    UIWord rx_;
    UIWord rj_; // non-negative, bytes [4, 5]
    UIWord ri_[k_ri_count]; // bytes [4, 5]
};

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
    (void)UIWordInput("A", ui_mix.ra_);
    (void)UIWordInput("X", ui_mix.rx_);
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

static void RenderAll()
{
    // ImGui::ShowDemoWindow(nullptr);

    static UIMix ui_mix;
    RegistersInputWindow(ui_mix);
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
