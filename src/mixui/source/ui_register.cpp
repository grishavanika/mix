#include <mixui/ui_register.h>

#include <imgui_internal.h>

#include <algorithm>

#include <cstring>

const char* SignText(mix::Sign sign)
{
    switch (sign)
    {
    case mix::Sign::Negative: return "-";
    case mix::Sign::Positive: return "+";
    }
    return "";
}

void UIRegister::set(const mix::Register& r)
{
    sign_ = r.sign();
    for (std::size_t i = 1; i <= mix::Register::k_bytes_count; ++i)
    {
        bytes_[i - 1] = r.byte(i).cast_to<int>();
    }
    value_ = r.value();
}

mix::Register UIRegister::get() const
{
    mix::Register r;
    r.set_sign(sign_);
    for (std::size_t i = 1; i <= mix::Register::k_bytes_count; ++i)
    {
        r.set_byte(i, bytes_[i - 1]);
    }
    return r;
}

static bool UISignInput(const char* label, mix::Sign* sign)
{
    mix::Sign selected_sign = *sign;

    auto do_sign = [&selected_sign
        , current_sign = *sign](mix::Sign draw_sign)
    {
        const bool active = (current_sign == draw_sign);
        if (!active)
        {
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha
                , ImGui::GetStyle().Alpha * 0.5f);
        }
        if (ImGui::ButtonEx(SignText(draw_sign), ImVec2()
            , ImGuiButtonFlags_Repeat | ImGuiButtonFlags_DontClosePopups))
        {
            selected_sign = draw_sign;
        }
        if (!active)
        {
            ImGui::PopStyleVar();
        }
    };

    ImGui::BeginGroup();
    ImGui::PushID(label);

    do_sign(mix::Sign::Negative); ImGui::SameLine();
    do_sign(mix::Sign::Positive);

    ImGui::PopID();
    ImGui::EndGroup();

    if (selected_sign != *sign)
    {
        *sign = selected_sign;
        return true;
    }
    return false;
}

bool UIRegisterInput(const char* title, UIRegister& state)
{
    bool changed = false;
    UIRegister::AllBytes old_bytes;
    std::memcpy(old_bytes, state.bytes_, sizeof(old_bytes));

    ImGui::BeginGroup();
    ImGui::PushID(title);

    // 1. Draw sign (+ and -)
    const float x_begin = ImGui::GetCursorPosX();
    changed |= UISignInput(title, &state.sign_);
    ImGui::SameLine();
    const float x_sign_end = ImGui::GetCursorPosX();

    // 2. Draw 5 bytes text input.
    // Using S32 to enable ImGui arithmetic (/2, *4).
    if (ImGui::InputScalarN(title, ImGuiDataType_S32, state.bytes_, IM_ARRAYSIZE(state.bytes_)))
    {
        for (int& byte : state.bytes_)
        {
            byte = std::clamp<int>(byte
                , mix::Byte::k_min_value
                , mix::Byte::k_max_value);
        }
        changed |= (std::memcmp(old_bytes, state.bytes_, sizeof(old_bytes)) != 0);
    }

    if (changed)
    {
        state.value_ = state.get().value();
    }

    // 3. Draw current sign label
    ImGui::SetCursorPosX(x_begin);
    ImGui::TextUnformatted(SignText(state.sign_));
    ImGui::SameLine();

    // 4. Draw big input text to change all the register.
    // Allow to set negative and positive values intentionally.
    // (Draw current sign to show the difference between -0 and +0).
    const ImS64 old_value = state.value_;
    ImGui::SetCursorPosX(x_sign_end);
    if (ImGui::InputScalar("##Value", ImGuiDataType_S64, &state.value_))
    {
        state.value_ = std::clamp<ImS64>(state.value_
            , -1 * static_cast<ImS64>(mix::Register::k_max_abs_value)
            , +1 * static_cast<ImS64>(mix::Register::k_max_abs_value));
        if (old_value != state.value_)
        {
            changed = true;
            state.set(mix::Register(static_cast<mix::WordValue::Type>(state.value_)));
        }
    }

    ImGui::EndGroup();
    ImGui::PopID();

    return changed;
}
