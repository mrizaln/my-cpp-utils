#include "move_only_function.hpp"

#include <iostream>

class Something
{
private:
    int m_value;

public:
    Something()                                  = delete;
    Something(const Something& other)            = delete;
    Something& operator=(const Something& other) = delete;

    Something(int value)
        : m_value{ value }
    {
        std::cout << "Something ctor\n";
    }

    Something(Something&& other) noexcept
        : m_value{ other.m_value }
    {
        std::cout << "Something move ctor\n";
    }

    Something& operator=(Something&& other) noexcept
    {
        m_value = other.m_value;
        std::cout << "Something move assign\n";
        return *this;
    }

    void print() const { std::cout << "Something: " << m_value << '\n'; }
    void add(int value) { m_value += value; }
    int  get() const { return m_value; }
};

void hello(int v)
{
    std::cout << v << " hello from free function\n";
}

void lambda()
{

    const auto func = [](int v) { std::cout << v << " hello from lambda\n"; };
    func(42);

    MoveOnlyFunction<void(int)> nFunc{ std::move(func) };
    nFunc(12);

    Something s{ 42 };

    MoveOnlyFunction<void()> funcc = [s = std::move(s)]() mutable {
        s.print();
        s.add(12);
    };

    funcc();

    const MoveOnlyFunction nFuncc{ std::move(funcc) };
    nFuncc();
}

void functor()
{
    struct Functor
    {
        Something m_s{ 0 };
        void      operator()(int v) { std::cout << m_s.get() + v << " functor non-const\n"; }
        void      operator()(int v) const { std::cout << m_s.get() + v << " functor const\n"; }
    };
    Something s{ 42 };

    Functor func{ .m_s = std::move(s) };
    func(234);

    const MoveOnlyFunction<void(int)> nFunc{ std::move(func) };
    nFunc(343);
}

void functionPtr()
{
    auto func = hello;
    func(63);

    const MoveOnlyFunction<void(int)> nFunc{ std::move(func) };
    nFunc(38);
}

int main()
{
    lambda();
    functor();
    functionPtr();

    return 0;
}
