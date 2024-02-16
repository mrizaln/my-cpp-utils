#include "enum_flag.hpp"

#include <bit>
#include <bitset>
#include <concepts>
#include <format>
#include <iostream>
#include <map>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

enum class FlagBit
{
    Flag1 = 1 << 0,
    Flag2 = 1 << 1,
    Flag3 = 1 << 2,
    Flag4 = 1 << 3,
    Flag5 = 1 << 4,
    Flag6 = 1 << 5,
    Flag7 = 1 << 6,
};

class Flag final : public EnumFlagBase<FlagBit, 1 + std::popcount((unsigned int)FlagBit::Flag7 - 1)>
{
private:
    inline static const std::map<FlagBit, std::string_view> s_names{
        // clang-format off
        { FlagBit::Flag1, "First" },
        { FlagBit::Flag2, "Second" },
        { FlagBit::Flag3, "Third" },
        { FlagBit::Flag4, "Fourth" },
        { FlagBit::Flag5, "Fifth" },
        { FlagBit::Flag6, "Sixth" },
        { FlagBit::Flag7, "Seventh" }
        // clang-format on
    };

public:
    Flag(std::convertible_to<FlagBit> auto... e)
        : EnumFlagBase{ e... }
    {
    }

public:
    const std::string str(bool withValues = false) const
    {
        constexpr std::size_t maxFlags{ sizeof(EnumBase) * 8 };
        std::size_t           usedFlags{ size() };

        std::string binaryRepresentation{
            std::bitset<8 * sizeof(EnumBase)>{ static_cast<std::size_t>(ord()) }.to_string()
        };
        std::string result{ std::format(
            "{0}[{1}]: ",
            binaryRepresentation.substr(0, maxFlags - usedFlags),
            binaryRepresentation.substr(maxFlags - usedFlags)
        ) };

        if (ord() > ALL || ord() < 0) {    // NOLINT: in case of signed base type used, ord() can be negative
            return result + "INVALID";
        }

        if (m_value == NONE) {
            if (withValues) {
                return result + std::format("{} [{}]", "NONE", ord());
            } else {
                return result + std::format("{}", "NONE");
            }
        } else if (m_value == ALL) {
            if (withValues) {
                return result + std::format("{} [{}]", "ALL", ord());
            } else {
                return result + std::format("{}", "ALL");
            }
        }

        bool first{ true };
        for (const auto& [val, name] : s_names) {
            auto uval{ static_cast<std::underlying_type_t<FlagBit>>(val) };
            if (m_value & uval) {
                if (!first) {
                    result.append(" | ");
                }
                first = false;

                if (withValues) {
                    result.append(std::format("{} [{}]", name, (std::size_t)val));
                } else {
                    result.append(std::format("{}", name, (std::size_t)val));
                }
            }
        }

        return result;
    }

    const std::map<FlagBit, std::string_view>& names() const { return s_names; }

    operator EnumBase() const { return ord(); }
    operator const std::string() const { return this->str(); }

    bool                 operator==(const Flag& other) { return m_value == other.m_value; }
    friend std::ostream& operator<<(std::ostream& out, const Flag& e) { return out << e.str(); }
};

class FlagWrapper
{
private:
    Flag m_flag;

public:
    FlagWrapper(std::same_as<FlagBit> auto&&... e)
        : m_flag{ std::forward<FlagBit>(e)... }
    {
    }

    Flag* operator->()
    {
        // print as precondition
        std::cout << m_flag << '\n';
        return &m_flag;
    }

    Flag& operator*() { return m_flag; }
};

void hello(const std::string& name)
{
    std::cout << "Hello " << name << '\n';
}

int main()
{
    FlagWrapper flag{ FlagBit::Flag1 };
    flag->toggle(FlagBit::Flag4);
    flag->toggleAll();
    flag->toggleAllExcept(FlagBit::Flag3);
    flag->toggleAllExcept(FlagBit::Flag5);
    flag->setAllExcept(FlagBit::Flag2);
    flag->toggle(FlagBit::Flag3);
    flag->setAllExcept(FlagBit::Flag6);
    flag->unsetAllExcept(FlagBit::Flag7);
    flag->reset();
    flag->base() = 320'743'279;
    flag->removeInvalidBits();
    flag->set(FlagBit::Flag1, FlagBit::Flag2, FlagBit::Flag3, FlagBit::Flag7);
    flag->toggleAll();
    flag->unset(FlagBit::Flag1, FlagBit::Flag4);
    flag->toggleAllExcept(FlagBit::Flag1, FlagBit::Flag2);
    flag->setAllExcept(FlagBit::Flag3, FlagBit::Flag4);
    flag->reset();
    flag->test(FlagBit::Flag3);
    std::cout << *flag << '\n';

    std::vector vec{ 1, 2, 3, 4 };
}
