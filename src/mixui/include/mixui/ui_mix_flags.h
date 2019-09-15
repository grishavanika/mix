#pragma once
#include <mix/general_types.h>

struct UIFlags
{
    int comparison_ = 0;
    bool overflow_ = false;

    static int FromComparisonIndicator(mix::ComparisonIndicator state);

    mix::ComparisonIndicator to_comparison_indicator() const;
    mix::OverflowFlag to_overflow_flag() const;

    void set(mix::ComparisonIndicator state, mix::OverflowFlag overflow);
};

bool UIFlagsInput(UIFlags& state);
