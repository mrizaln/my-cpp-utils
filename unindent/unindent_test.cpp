#include "unindent.hpp"
#include <iostream>

constexpr auto string = R"python(
    def main():
        with open(__file__) as f:
            for line in f:
                print(line, end="")

    if __name__ == "__main__":
        main()
)python";

int main()
{
    std::cout << unindent(string, true, true) << '\n';
    return 0;
}
