#ifndef MATCH_HPP
#define MATCH_HPP

#ifdef match
#    error "match macro already defined!"
#endif

#include <concepts>
#include <utility>
#include <variant>

namespace variant_helper
{
    template <typename... Fs>
    struct overloaded : Fs...
    {
        using Fs::operator()...;
    };

    // order does not matter and allow implicit conversion of the value to the lambda inside the overload set
    template <typename... Ts, typename... Fs>
        requires(std::invocable<overloaded<Fs...>, Ts&> && ...)
    auto operator|(std::variant<Ts...>* variant, overloaded<Fs...>&& dummy)
    {
        return std::visit(std::forward<overloaded<Fs...>>(dummy), *variant);
    }
    template <typename... Ts, typename... Fs>
        requires(std::invocable<overloaded<Fs...>, const Ts&> && ...)
    auto operator|(const std::variant<Ts...>* variant, overloaded<Fs...>&& dummy)
    {
        return std::visit(std::forward<overloaded<Fs...>>(dummy), *variant);
    }

    // order does matter, implicit conversion may still happen but sort of mitigated by the fact that the overload set
    // must handle all the types on the variant
    template <typename... Ts, typename... Fs>
        requires(std::invocable<Fs, Ts&> && ...)
    auto operator*(std::variant<Ts...>* variant, overloaded<Fs...>&& dummy)
    {
        return std::visit(std::forward<overloaded<Fs...>>(dummy), *variant);
    }

    template <typename... Ts, typename... Fs>
        requires(std::invocable<Fs, const Ts&> && ...)
    auto operator*(const std::variant<Ts...>* variant, overloaded<Fs...>&& dummy)
    {
        return std::visit(std::forward<overloaded<Fs...>>(dummy), *variant);
    }
}

#define match(variant) &variant | variant_helper::overloaded
#define matchall(variant) &variant * variant_helper::overloaded

#endif /* end of include guard: MATCH_HPP */
