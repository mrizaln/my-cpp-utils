#include "strong_variant.hpp"
#include "match.hpp"
#include "type_name.hpp"

#include <algorithm>
#include <array>
#include <concepts>
#include <format>
#include <iostream>
#include <optional>
#include <random>
#include <string>
#include <sstream>

struct foo
{
};
struct bar
{
};

struct A
{
    std::string          a;
    friend std::ostream& operator<<(std::ostream& os, const A& a) { return os << a.a; }
};

struct B
{
    std::array<int, 3>   b;
    friend std::ostream& operator<<(std::ostream& os, const B& b)
    {
        return os << "{ " << b.b[0] << ' ' << b.b[1] << ' ' << b.b[2] << " }";
    }
};

using namespace std::literals;

void useStrongVariant()
{
    foo       f;
    bar       b;
    const foo c_f;

    StrongVariant<foo*, std::string, bool> sv;

    sv = &f;        // okay.
    sv = true;      // okay.
    sv = "foo"s;    // okay.

    // sv = "foo";    // no, must a string.
    // sv = &b;       // no, must be a foo.
    // sv = &c_f;     // no, must be non-const.
}

void useMatch()
{
    // strong variant prevents implicit conversion
    using Variant = StrongVariant<bool, int, A, B, std::string, std::optional<int>, foo, bar>;

    std::array<Variant, 35> variants{
        1,  A{ "hello" },      "The quick"s,  B{ { 1, 2, 3 } },    std::optional<int>{},     true,  foo{},
        42, A{ "world" },      "brown fox"s,  B{ { 4, 5, 6 } },    std::optional<int>{ 42 }, false, bar{},
        2,  A{ "!" },          "jumps over"s, B{ { 7, 8, 9 } },    std::optional<int>{},     false, bar{},
        12, A{ "AaaaAAaaAA" }, "the lazy"s,   B{ { 10, 11, 12 } }, std::optional<int>{},     false, foo{},
        60, A{ "BbbbBBbbBB" }, "dog"s,        B{ { 13, 14, 15 } }, std::optional<int>{ -1 }, true,  bar{},
    };
    std::shuffle(variants.begin(), variants.end(), std::mt19937{ std::random_device{}() });

    const auto printWithTypeName = [](auto&& value) {
        constexpr std::string_view fmt{ "{:>40} = {}\n" };
        using Value = std::decay_t<decltype(value)>;
        if constexpr (std::same_as<Value, bool>) {
            std::cout << std::format(fmt, type_name(value), value ? "true" : "false");
        } else if constexpr (std::is_same_v<Value, std::optional<int>>) {
            if (value) {
                std::cout << std::format(fmt, type_name(value), value.value());
            } else {
                std::cout << std::format(fmt, type_name(value), "nullopt");
            }
        } else if constexpr (requires(std::stringstream ss) { ss << value; }) {
            std::stringstream ss;
            ss << value;
            std::cout << std::format(fmt, type_name(value), ss.str());
        } else if constexpr (std::convertible_to<Value, std::string>) {
            std::cout << std::format(fmt, type_name(value), value);
        } else {
            std::cout << std::format(fmt, type_name(value), "<cannot_format_value>");
        }
    };

    // strong variant however still can't prevent the visitor to receive implicitly converted value
    for (const auto& variant : variants) {
        // clang-format off
        matchall(variant){
            [&](bool b)                      { printWithTypeName(b); },
            [&](int i)                       { printWithTypeName(i); },
            [&](const A& a)                  { printWithTypeName(a); },
            [&](const B& b)                  { printWithTypeName(b); },
            [&](const std::string& s)        { printWithTypeName(s); },
            [&](const std::optional<int>& o) { printWithTypeName(o); },
            [&](const foo& f)                { printWithTypeName(f); },
            [&](const bar& b)                { printWithTypeName(b); },
        };

        // match(variant) { printWithTypeName };
        // clang-format on
    }
}

int main()
{
    useStrongVariant();
    useMatch();
}
