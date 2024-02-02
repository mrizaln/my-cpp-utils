#ifndef TYPE_NAME_H
#define TYPE_NAME_H

#include <string_view>

#if !defined(__clang__) && !defined(__GNUC__) && !defined(_MSC_VER_)
#error "Can't be used on compiler other than GCC, Clang, and MSVC"
#else

template <typename T>
constexpr auto type_name()
{
    std::string_view name;
    std::string_view prefix;
    std::string_view suffix;

#ifdef __clang__
    name   = __PRETTY_FUNCTION__;
    prefix = "auto type_name() [T = ";
    suffix = "]";
#elif defined(__GNUC__)
    name   = __PRETTY_FUNCTION__;
    prefix = "constexpr auto type_name() [with T = ";
    suffix = "]";
#elif defined(_MSC_VER)
    name   = __FUNCSIG__;
    prefix = "auto __cdecl type_name<";
    suffix = ">(void)";
#endif
    name.remove_prefix(prefix.size());
    name.remove_suffix(suffix.size());
    return name;
}

template <typename T>
constexpr auto type_name(T&&) { return type_name<T>(); }

#endif

#endif /* end of include guard: TYPE_NAME_H */
