#include <cstdio>
#include <iostream>
#include <string_view>

#include "defer.hpp"

std::string getSource()
{
    FILE* f = fopen(__FILE__, "r");
    if (!f) {
        return {};
    }
    defer {
        fclose(f);
    };

    fseek(f, 0, SEEK_END);
    auto length = static_cast<std::size_t>(ftell(f));

    fseek(f, 0, SEEK_SET);

    std::string string(length + 1, '\0');
    fread(string.data(), 1, length, f);

    return string;
}

int main()
{
    constexpr std::string_view hello = "hello deferred";

    defer {
        std::cout << getSource() << '\n';
    };

    defer {
        std::cout << hello << '\n';
        std::cout << "other defer\n";
    };

    defer {
        std::cout << "defer2\n";
    };

    std::cout << "hello world\n";

    return 0;
}
