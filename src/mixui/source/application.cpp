#include <mixui/application.h>
#include <mixui/command_help.h>

#include <mix/default_device.h>

#include <imgui.h>
#include <imgui_stdlib.h>

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

static void UIControlsModifyMix(UIMix* ui_mix, mix::Computer* mix, Debugger* debugger)
{
    // Temporary code to simplify life. Once debugging
    // requested, make sure we do have loaded & running program.
    const bool requires_running = ui_mix->controls_.run_one_
        || ui_mix->controls_.run_to_breakpoint_;
    if (requires_running)
    {
        if (!debugger->loaded_ || mix->is_halted())
        {
            ui_mix->controls_.load_from_file = true;
        }
    }

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
        } while (!mix->is_halted()
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
        // #XXX: do not jump. Simply modify rJ
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

static void UIRenderOutputAndHelp(const mix::Computer& mix
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

void OnKeyboardF5(Application* app)
{
    app->ui_mix_.controls_.run_to_breakpoint_ = true;
    UIControlsModifyMix(&app->ui_mix_, &app->mix_, &app->debugger_);
}

void OnKeyboardF10(Application* app)
{
    app->ui_mix_.controls_.run_one_ = true;
    UIControlsModifyMix(&app->ui_mix_, &app->mix_, &app->debugger_);
}

void RenderAll(Application* app)
{
#if (0)
    ImGui::ShowDemoWindow(nullptr);
#endif

    MixModifyUI(app->mix_, &app->ui_mix_);
    UIRegistersInputModifyMix(&app->ui_mix_, &app->mix_);
    UIFlagsAndAddressInputModifyMix(&app->ui_mix_, &app->mix_);
    UIControlsInput(&app->ui_mix_.controls_, app->mix_, app->debugger_);
    UIControlsModifyMix(&app->ui_mix_, &app->mix_, &app->debugger_);
    UIDebuggerViewInput(app->mix_, app->debugger_, &app->ui_mix_.debugger_view_);
    UIDebuggerModifyMix(app->ui_mix_.debugger_view_, &app->debugger_, &app->mix_);
    UIRenderOutputAndHelp(app->mix_, mix::CommandHelp(app->mix_processor_), app->debugger_);
}

