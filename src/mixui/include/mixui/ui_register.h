#pragma once
#include <mix/general_types.h>
#include <mix/registers.h>

#include <imgui.h>

struct UIRegister
{
    static_assert(sizeof(int) >= sizeof(mix::Byte::NarrowType)
        , "Type of int used to save MIX byte on UI side"
        " is not enough to store whole MIX Byte value.");

    static constexpr int k_bytes_count = static_cast<int>(mix::Register::k_bytes_count);
    using AllBytes = int[k_bytes_count];

public:
    mix::Sign sign_ = mix::Sign::Positive;
    AllBytes bytes_{};
    ImS64 value_ = 0;

public:
    void set(const mix::Register& r);
    mix::Register get() const;
};

bool UIRegisterInput(const char* name, UIRegister& state);

const char* SignText(mix::Sign sign);

