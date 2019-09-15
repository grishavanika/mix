#pragma once
#include <mixui/ui_word.h>
#include <mixui/ui_mix_flags.h>
#include <mixui/ui_debugger_view.h>
#include <mixui/debugger.h>

#include <mix/computer.h>
#include <mix/command_processor.h>

#include <string>

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

struct Application
{
    mix::Computer mix_;
    mix::CommandProcessor mix_processor_;
    Debugger debugger_;
    UIMix ui_mix_;

    Application()
        : mix_()
        , mix_processor_(mix_)
        , debugger_()
        , ui_mix_()
    {
    }
};

void RenderAll(Application* app);
void OnKeyboardF5(Application* app);
void OnKeyboardF10(Application* app);

