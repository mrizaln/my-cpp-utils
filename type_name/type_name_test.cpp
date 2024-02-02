#include <iostream>
#include <string>

#include "type_name.hpp"

int main()
{
    std::string       hello{ "HI" };
    unsigned int      a{ 2319 };
    const auto* const b{ &a };

    std::cout << type_name(hello) << '\n';
    std::cout << type_name(a) << '\n';
    std::cout << type_name(b) << '\n';
    std::cout << type_name<unsigned long long>() << '\n';
}
