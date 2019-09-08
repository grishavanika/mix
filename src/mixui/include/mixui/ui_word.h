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

// `start` and `stop` represent WordFiedl,
// i.e, [1, 5] - whole word, [4, 5] - last 2 bytes
bool UIWordInputEx(const char* name, UIWord& state
    , int start, int stop, bool allow_negative);

inline bool UIWordInput(const char* name, UIWord& state)
{
    return UIWordInputEx(name, state
        , 1/*start byte*/
        , UIWord::k_bytes_count/*end byte*/
        , true/*allow negative values*/);
}

inline bool UIRegisterInput(const char* name, UIWord& state)
{
    return UIWordInputEx(name, state
        , 1/*start byte*/
        , UIWord::k_bytes_count/*end byte*/
        , true/*allow negative values*/);
}

inline bool UIIndexRegisterInput(const char* name, UIWord& state)
{
    static_assert(UIWord::k_bytes_count >= 4);
    return UIWordInputEx(name, state
        , 4/*start byte*/
        , UIWord::k_bytes_count/*end byte*/
        , true/*allow negative values*/);
}

inline bool UIAddressRegisterInput(const char* name, UIWord& state)
{
    static_assert(UIWord::k_bytes_count >= 4);
    return UIWordInputEx(name, state
        , 4/*start byte*/
        , UIWord::k_bytes_count/*end byte*/
        , false/*disable negative values*/);
}

const char* SignText(mix::Sign sign);

