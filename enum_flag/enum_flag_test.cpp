#include "enum_flag.hpp"

#include <bitset>
#include <iostream>

enum class FlagBit
{
    Flag1 = 1 << 0,
    Flag2 = 1 << 1,
    Flag3 = 1 << 2,
    Flag4 = 1 << 3,
};

// simple alias to use EnumFlagBase conveniently
using FlagBase = EnumFlagBase<FlagBit, 4>;

// or define a new class to use EnumFlagBase by deriving from it
class Flag final : public FlagBase
{
public:
    Flag(std::convertible_to<FlagBit> auto... e)
        : EnumFlagBase{ e... }
    {
    }

    friend std::ostream& operator<<(std::ostream& os, const Flag& f) { return os << std::bitset<4>(f.ord()); }
};

int main()
{
    auto e = FlagBit::Flag1;

    FlagBase flag{ FlagBit::Flag3, FlagBit::Flag1, e };
    std::cout << std::bitset<flag.size()>(flag.ord()) << '\n';

    Flag f{ FlagBit::Flag2, e };
    f.set(FlagBit::Flag4, FlagBit::Flag1, e);
    std::cout << f << '\n';
}
