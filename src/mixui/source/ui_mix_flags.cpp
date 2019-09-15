#include <mixui/ui_mix_flags.h>

#include <imgui.h>

/*static*/ int UIFlags::FromComparisonIndicator(mix::ComparisonIndicator state)
{
    switch (state)
    {
    case mix::ComparisonIndicator::Less   : return 1;
    case mix::ComparisonIndicator::Greater: return 2;
    case mix::ComparisonIndicator::Equal  : return 0;
    }
    return 0;
}

mix::ComparisonIndicator UIFlags::to_comparison_indicator() const
{
    switch (comparison_)
    {
    case 1: return mix::ComparisonIndicator::Less;
    case 2: return mix::ComparisonIndicator::Greater;
    case 0: return mix::ComparisonIndicator::Equal;
    }
    return mix::ComparisonIndicator::Less;
}

mix::OverflowFlag UIFlags::to_overflow_flag() const
{
    return overflow_
        ? mix::OverflowFlag::Overflow
        : mix::OverflowFlag::NoOverflow;
}

void UIFlags::set(mix::ComparisonIndicator state, mix::OverflowFlag overflow)
{
    comparison_ = FromComparisonIndicator(state);
    overflow_ = (overflow == mix::OverflowFlag::Overflow);
}

bool UIFlagsInput(UIFlags& state)
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
