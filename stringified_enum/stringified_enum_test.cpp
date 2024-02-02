#include "stringified_enum.hpp"
#include "stringified_enum_flag.hpp"

#include <iostream>

// stringified_enum_as_flags_improved
#define FIELDS(M) \
    M(Flag1),     \
    M(Flag2),     \
    M(Flag3),     \
    M(Flag4),     \
    M(Flag5),     \
    M(Flag6),     \
    M(Flag7),     \
    M(Flag8),     \
    M(Flag9),     \
    M(Flag10),    \
    M(Flag11),    \
    M(Flag12)
STRINGIFIED_ENUM_FLAG(Flag, unsigned short, FIELDS);
#undef FIELDS

// stringified_enum_as_flags_improved
#define FIELDS(M) \
    M(Flag1, "first"),      \
    M(Flag2, "second"),     \
    M(Flag3, "third"),      \
    M(Flag4, "fourth"),     \
    M(Flag5, "fifth"),      \
    M(Flag6, "sixth"),      \
    M(Flag7, "seventh")
using FlagToo = STRINGIFIED_ENUM_FLAG_STRING(FlagToo, char, FIELDS);
#undef FIELDS

// stringified_enum
#define FIELDS(M)  \
    M(Flag1, "asdfiaweofhaskl"), \
    M(Flag2, "gwproeghwaeuhdj"),
STRINGIFIED_ENUM_STRING(Enum, int, FIELDS);
#undef FIELDS

// stringified_enum
#define FIELDS(M) \
    M(Flag1),     \
    M(Flag2),     \
    M(Flag3),     \
    M(Flag4),     \
    M(Flag5),
using Enum2 = STRINGIFIED_ENUM(Enum2, int, FIELDS);
#undef FIELDS

int main()
{
    for (const auto& [value, name] : Flag::s_enums) {
        std::cout << std::format("{0} \t= {1:0{2}b}\n", name, value, Flag::size());
    }
    std::cout << '\n';

    for (const auto& [value, name] : FlagToo::s_enums) {
        std::cout << std::format("{0} \t= {1:0{2}b}\n", name, value, FlagToo::size());
    }
    std::cout << '\n';

    Flag flag1{};
    std::cout << flag1.set(Flag::Flag1, Flag::Flag3, Flag::Flag8).str(true) << '\n';
    std::cout << flag1.set(Flag::ALL).str(true) << '\n';
    std::cout << flag1.unset(Flag::Flag5).str(true) << '\n';

    std::cout << "\n>>> using set(), unset(), and toggle()\n";
    flag1.reset();
    flag1.set(Flag::Flag2).set(Flag::Flag4).set(Flag::Flag5);
    std::cout << "set   : " << flag1 << '\n';
    flag1.unset(Flag::Flag1, Flag::Flag3, Flag::Flag7, Flag::Flag4, Flag::Flag5);
    flag1.unset(Flag::NONE);    // do nothing
    std::cout << "unset : " << flag1 << '\n';
    flag1.set(Flag::Flag1, Flag::Flag3, Flag::Flag7, Flag::Flag4, Flag::Flag5);
    std::cout << "set   : " << flag1 << '\n';
    flag1.reset();
    std::cout << "reset : " << flag1 << '\n';
    flag1.set(Flag::ALL);
    std::cout << "set   : " << flag1 << '\n';
    flag1.toggle(Flag::Flag7, Flag::Flag2, Flag::Flag6);
    std::cout << "toggle: " << flag1 << '\n';
    flag1.toggle(Flag::ALL);
    std::cout << "toggle: " << flag1 << '\n';

    std::cout << "\n>>> using test()\n";
    flag1.reset();
    flag1.set(Flag::Flag1, Flag::Flag2, Flag::Flag3, Flag::Flag4, Flag::Flag5);
    flag1.toggle(Flag::ALL);
    std::cout << flag1.str(true) << '\n';
    std::cout << "none contain : " << flag1.test(Flag::Flag2, Flag::Flag3, Flag::Flag4, Flag::Flag5) << '\n';
    std::cout << "some contain : " << flag1.test(Flag::Flag6, Flag::Flag4, Flag::Flag5) << '\n';
    std::cout << "all contain  : " << flag1.test(Flag::Flag6, Flag::Flag8, Flag::Flag9) << '\n';
    std::cout << "flag2 : " << flag1.test(Flag::Flag2) << '\n';
    std::cout << "flag6 : " << flag1.test(Flag::Flag6) << '\n';
    std::cout << "none  : " << flag1.test(Flag::NONE) << '\n';
    std::cout << "all   : " << flag1.test(Flag::ALL) << '\n';
    flag1.reset();
    std::cout << flag1.str(true) << '\n';
    std::cout << "none  : " << flag1.test(Flag::NONE) << '\n';
    std::cout << "all   : " << flag1.test(Flag::ALL) << '\n';
    flag1.set(Flag::ALL);
    std::cout << flag1.str(true) << '\n';
    std::cout << "none  : " << flag1.test(Flag::NONE) << '\n';
    std::cout << "all   : " << flag1.test(Flag::ALL) << '\n';

    std::cout << "\n>>> using testSome()\n";
    flag1.unset(Flag::Flag1, Flag::Flag2, Flag::Flag3, Flag::Flag4, Flag::Flag5);
    std::cout << flag1.str(true) << '\n';
    std::cout << "none contain : " << flag1.testSome(Flag::Flag2, Flag::Flag3, Flag::Flag4, Flag::Flag5) << '\n';
    std::cout << "some contain : " << flag1.testSome(Flag::Flag6, Flag::Flag4, Flag::Flag5) << '\n';
    std::cout << "all contain  : " << flag1.testSome(Flag::Flag6, Flag::Flag8, Flag::Flag9) << '\n';
    std::cout << "flag2 : " << flag1.testSome(Flag::Flag2) << '\n';
    std::cout << "flag6 : " << flag1.testSome(Flag::Flag6) << '\n';
    std::cout << "none  : " << flag1.testSome(Flag::NONE) << '\n';
    std::cout << "all   : " << flag1.testSome(Flag::ALL) << '\n';
    flag1.reset();
    std::cout << flag1.str(true) << '\n';
    std::cout << "none  : " << flag1.testSome(Flag::NONE) << '\n';
    std::cout << "all   : " << flag1.testSome(Flag::ALL) << '\n';
    flag1.set(Flag::ALL);
    std::cout << flag1.str(true) << '\n';
    std::cout << "none  : " << flag1.testSome(Flag::NONE) << '\n';
    std::cout << "all   : " << flag1.testSome(Flag::ALL) << '\n';

    const FlagToo cat{ FlagToo::Flag1 };
    std::cout << "cat ord: " << cat.ord() << '\n';

    // this use of switch case on flag is ill formed (a flag enum can be a composite of multiple flags or enum values)
    switch (cat) {
    case FlagToo::Flag1:
    case FlagToo::Flag2:
    case FlagToo::Flag3:
    case FlagToo::Flag4:
        break;
    [[unlikely]] case FlagToo::ALL:
        break;
    case FlagToo::NONE:
        break;
    case FlagToo::Flag5:
    case FlagToo::Flag6:
    case FlagToo::Flag7:
        break;
    }

    std::cout << std::format("sizeof {}: {}\n", Flag::s_className, sizeof(Flag));
    std::cout << std::format("sizeof {}: {}\n", FlagToo::s_className, sizeof(FlagToo));

    FlagToo enn{ FlagToo::Flag2, FlagToo::Flag7, FlagToo::Flag5, FlagToo::Flag1, FlagToo::Flag4 };
    std::cout << enn.str(true) << '\n';
    // enn.base() = std::numeric_limits<FlagToo::Base_type>::min();
    enn.base() = -103;
    std::cout << enn.str(true) << '\n';

    // stringified_enum
    Enum a{ Enum::Flag2 };
    std::cout << Enum::s_className << ": " << a.str() << '\n';

    Enum2 b{ Enum2::Flag3 };
    std::cout << Enum2::s_className << ": " << b.str() << '\n';


    return 0;
}
