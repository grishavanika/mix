#pragma once
#include <mix/general_types.h>
#include <mix/word.h>

#include <imgui.h>

struct UIWord
{
    static_assert(sizeof(int) >= sizeof(mix::Byte::NarrowType)
        , "Type of int used to save MIX byte on UI side"
        " is not enough to store whole MIX Byte value.");

    static constexpr int k_bytes_count = static_cast<int>(mix::Word::k_bytes_count);
    using AllBytes = int[k_bytes_count];

public:
    mix::Sign sign_ = mix::Sign::Positive;
    AllBytes bytes_{};
    ImS64 value_ = 0;

public:
    void set(const mix::Word& r);
    mix::Word get() const;
};

bool UIWordInput(const char* name, UIWord& state
    , int start = 1, int stop = UIWord::k_bytes_count
    , bool allow_negative = true);

inline bool UIRegisterInput(const char* name, UIWord& state)
{
    return UIWordInput(name, state);
}

inline bool UIIndexRegisterInput(const char* name, UIWord& state)
{
    return UIWordInput(name, state
        , 4/*start byte*/
        , 5/*end byte*/);
}

inline bool UIAddressRegisterInput(const char* name, UIWord& state)
{
    return UIWordInput(name, state
        , 4/*start byte*/
        , 5/*end byte*/
        , false/*allow non-negative values only*/);
}

const char* SignText(mix::Sign sign);

