#ifndef NOT_NULL_HPP
#define NOT_NULL_HPP

#include <cassert>
#include <cstddef>
#include <ostream>
#include <type_traits>

#ifndef NOTNULL_RUNTIME_NULL_CHECK_ASSERT
#    define NOTNULL_RUNTIME_NULL_CHECK_ASSERT 0
#endif
#ifndef NOTNULL_RUNTIME_NULL_CHECK_EXCEPTION
#    define NOTNULL_RUNTIME_NULL_CHECK_EXCEPTION 0
#endif

template <typename T>
    requires std::is_pointer_v<T>
class NotNull
{
public:
    using Pointer   = T;
    using Element   = std::remove_pointer_t<T>;
    using Reference = Element&;

public:
    constexpr explicit NotNull(Pointer ptr)
        : m_ptr{ checkNull(ptr) }
    {
    }
    constexpr NotNull(std::nullptr_t)            = delete;
    constexpr NotNull(const NotNull&)            = default;
    constexpr NotNull(NotNull&&)                 = default;
    constexpr NotNull& operator=(std::nullptr_t) = delete;

    constexpr NotNull& operator=(const NotNull& other) { m_ptr = checkNull(other.m_ptr); }
    constexpr NotNull& operator=(NotNull&& other) { m_ptr = checkNull(other.m_ptr); }

    constexpr NotNull& operator=(Pointer ptr)
    {
        m_ptr = checkNull(ptr);
        return *this;
    }

    constexpr operator Pointer() const { return m_ptr; }

    constexpr void* operator new(std::size_t)   = delete;
    constexpr void* operator new[](std::size_t) = delete;
    constexpr void operator delete(void*)       = delete;
    constexpr void operator delete[](void*)     = delete;

    constexpr Pointer   get() const { return m_ptr; }
    constexpr Pointer   operator->() const { return m_ptr; }
    constexpr Reference operator*() const { return *m_ptr; }

    constexpr friend std::ostream& operator<<(std::ostream& os, const NotNull& nn) { return os << nn.m_ptr; }

private:
    constexpr static Pointer checkNull(Pointer ptr)
    {
        // unfortunately Pointer may be null even though the type is Pointer, we can't rely on NotNull(std::nullptr_t)
        // being deleted for this case
        if constexpr (NOTNULL_RUNTIME_NULL_CHECK_EXCEPTION == 1) {
            if (ptr == nullptr) {
                throw std::runtime_error{ "Pointer contains nullptr. This might be a bug in the code!" };
            }
        } else if constexpr (NOTNULL_RUNTIME_NULL_CHECK_ASSERT == 1) {
            assert(ptr != nullptr && "Pointer contains nullptr. This might be a bug in the code!");
        }
        return ptr;
    }

private:
    Pointer m_ptr;
};

#endif /* end of include guard: NOT_NULL_HPP */
