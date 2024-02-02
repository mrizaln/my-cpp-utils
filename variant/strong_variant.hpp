#ifndef STRONG_VARIANT_HPP
#define STRONG_VARIANT_HPP

#include <concepts>
#include <variant>

// https://stackoverflow.com/a/64295708
template <typename... Ts>
class StrongVariant : public std::variant<Ts...>
{
public:
    template <typename T>
        requires(std::same_as<T, Ts> || ...)
    StrongVariant(T v)
        : std::variant<Ts...>(v)
    {
    }

    StrongVariant()
        : std::variant<Ts...>()
    {
    }
};

#endif /* end of include guard: STRONG_VARIANT_HPP */
