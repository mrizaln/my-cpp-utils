#include <cstdint>
#include <iostream>

#include "stringified_enum_flag.hpp"

// clang-format off
#define FIELDS(M) \
    M(Flag1),     \
    M(Flag2),     \
    M(Flag3),     \
    M(Flag4)

STRINGIFIED_ENUM_FLAG(Flag, uint8_t, FIELDS);
#undef FIELDS

#define FIELDS(M) \
    M(Flag1, "adsfk sadkfjh adkfj"),\
    M(Flag2, "dsafjhu adskfgoiuss"),\
    M(Flag3, "fasd7gj adsigf jhil"),\
    M(Flag4, "sdfig3pudlh sdajhfj")
STRINGIFIED_ENUM_FLAG_STRING(FlagToo, unsigned short, FIELDS);
#undef FIELDS
// clang-format on

int main()
{
    Flag flag{ Flag::Flag2, Flag::Flag3 };
    std::cout << flag << '\n';

    FlagToo flag2{ FlagToo::Flag1, FlagToo::Flag4 };
    std::cout << flag2 << '\n';
}
