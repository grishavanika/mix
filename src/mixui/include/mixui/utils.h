#include <cmath>

inline int GetDigitsCount(int n)
{
    return static_cast<int>(std::floor(std::log10(n) + 1));
}
