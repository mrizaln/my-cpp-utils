// #define NOTNULL_RUNTIME_NULL_CHECK_ASSERT 1
#define NOTNULL_RUNTIME_NULL_CHECK_EXCEPTION 1
#include "not_null.hpp"
#include "type_name.hpp"

#include <iostream>

struct A
{
    int a;
    int b;

    void print() const { std::cout << a << ' ' << b << '\n'; }
};

int main()
{
    int        a{ 42 };
    int        b{ a };
    int&       c{ b };
    const int  d{ 12 };
    const int  e{ d };
    const int& f{ e };
    const int& g{ a };
    const int& h{ c };

    const int* p{ &a };

    NotNull<int*> ptr{ &a };
    ptr = &b;
    ptr = &c;
    // ptr = &d;
    // ptr = &e;
    // ptr = &f;
    // ptr = &g;
    // ptr = &h;
    // ptr = p;

    NotNull<const int*> ptr2{ &e };
    ptr2 = &b;
    ptr2 = &c;
    ptr2 = &d;
    ptr2 = &e;
    ptr2 = &f;
    ptr2 = &g;
    ptr2 = &h;
    ptr2 = p;

    std::cout << &a << ' ' << ptr << ' ' << &b << '\n';

    NotNull<A*> ptr3{ new A{ 1, 2 } };
    ptr3->print();

    NotNull<const A*> ptr4{ new A{ 3, 4 } };
    ptr4->print();
    delete ptr4.get();

    ptr4 = ptr3;
    ptr4->print();
    delete ptr4.get();

    // inserting a nullptr will fail to compile
    // NotNull<A*> ptr5{ nullptr };

    // but a pointer that contains nullptr will be able to be inserted.
    // if the NOTNULL_RUNTIME_NULL_CHECK_EXCEPTION is defined to 1, the constructor will throw if it receives a pointer
    // that points to nullptr.
    try {
        int*          p2{ nullptr };
        NotNull<int*> ptr6{ p2 };    // throws
    } catch (std::exception& e) {
        std::cerr << "EXCEPTION: " << e.what() << '\n';
    }

    std::cout << "sizeof " << type_name(ptr) << ": " << sizeof ptr << '\n';
}
