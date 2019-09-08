#include <mixui/ui_word.h>

#include <imgui_internal.h>

#include <algorithm>

#include <cassert>
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

void UIWord::set(const mix::Word& w)
{
    sign_ = w.sign();
    for (std::size_t i = 1; i <= mix::Word::k_bytes_count; ++i)
    {
        bytes_[i - 1] = w.byte(i).cast_to<int>();
    }
    value_ = w.value();
}

mix::Word UIWord::get() const
{
    mix::Word w;
    w.set_sign(sign_);
    for (std::size_t i = 1; i <= mix::Word::k_bytes_count; ++i)
    {
        w.set_byte(i, bytes_[i - 1]);
    }
    return w;
}

static bool UISignInput(const char* label, mix::Sign* sign = nullptr)
{
    mix::Sign selected_sign = sign ? *sign : mix::Sign::Positive;

    auto do_sign = [&selected_sign, sign](mix::Sign draw_sign)
    {
        const bool active = sign && (draw_sign == *sign);
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

    if (sign && (selected_sign != *sign))
    {
        *sign = selected_sign;
        return true;
    }
    return false;
}

static constexpr int MaxValueInMixBytes(int bytes_count)
{
    const std::size_t field_bits = static_cast<std::size_t>(bytes_count) * mix::Byte::k_bits_count;
    assert(field_bits <= (sizeof(int) * CHAR_BIT));
    return static_cast<int>((std::size_t(1) << field_bits) - 1);
}

// #XXX: AddressRegister should disallow sign (always positive)
bool UIWordInput(const char* title, UIWord& state
    , int start /*= 1*/, int stop /*= UIWord::k_bytes_count*/
    , bool allow_negative /*= true*/)
{
    assert(start >= 1);
    assert(stop <= UIWord::k_bytes_count);
    assert(start <= stop);

    const int bytes_count = (stop - start + 1);
    assert(bytes_count <= IM_ARRAYSIZE(state.bytes_));

    bool changed = false;
    UIWord::AllBytes old_bytes;
    std::memcpy(old_bytes, state.bytes_, sizeof(old_bytes));

    ImGui::BeginGroup();
    ImGui::PushID(title);

    // 1. Draw sign (+ and -)
    if (!allow_negative)
    {
        state.sign_ = mix::Sign::Positive;
    }
    const float x_begin = ImGui::GetCursorPosX();
    changed |= UISignInput(title, allow_negative
        ? &state.sign_
        : nullptr);
    ImGui::SameLine();
    const float x_sign_end = ImGui::GetCursorPosX();

    // 2. Draw 5 bytes text input.
    // Using S32 to enable ImGui arithmetic (/2, *4).
    if (ImGui::InputScalarN(title, ImGuiDataType_S32
        , state.bytes_ + start - 1
        , bytes_count))
    {
        for (int i = start; i <= stop; ++i)
        {
            int& byte = state.bytes_[i - 1];
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

    // 4. Draw big input text to change all the word.
    // Allow to set negative and positive values intentionally.
    // (Draw current sign to show the difference between -0 and +0).
    const int max_value = 1 * MaxValueInMixBytes(bytes_count);
    const int min_value = (allow_negative ? -1 : 0)* max_value;
    const ImS64 old_value = state.value_;
    ImGui::SetCursorPosX(x_sign_end);
    if (ImGui::InputScalar("##Value", ImGuiDataType_S64, &state.value_))
    {
        state.value_ = std::clamp<ImS64>(state.value_, min_value, max_value);
        if (old_value != state.value_)
        {
            changed = true;
            state.set(mix::Word(static_cast<mix::WordValue::Type>(state.value_)));
        }
    }

    ImGui::EndGroup();
    ImGui::PopID();

    return changed;
}
