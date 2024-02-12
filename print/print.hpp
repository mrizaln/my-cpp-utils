#ifndef PRINT_HPP_985ECFRWB6T
#define PRINT_HPP_985ECFRWB6T

#ifndef PRINT_USE_IOSTREAM
#    define PRINT_USE_IOSTREAM 0
#endif

#include <iostream>
#include <format>
#include <utility>

template <typename... Args>
void print(std::format_string<Args...>&& fmt, Args&&... args)
{
    std::string str = std::format(std::forward<std::format_string<Args...>>(fmt), std::forward<Args>(args)...);
    if constexpr (PRINT_USE_IOSTREAM) {
        std::cout << str;
    } else {
        std::fwrite(str.data(), sizeof(std::string::value_type), str.size(), stdout);
    }
}

template <typename... Args>
void println(std::format_string<Args...>&& fmt, Args&&... args)
{
    print(std::forward<std::format_string<Args...>>(fmt), std::forward<Args>(args)...);
    if constexpr (PRINT_USE_IOSTREAM) {
        std::cout << '\n';
    } else {
        std::putc('\n', stdout);
    }
}

#endif /* end of include guard: PRINT_HPP_985ECFRWB6T */
