#ifndef MOVE_ONLY_FUNCTION_HPP_4SABI1FX
#define MOVE_ONLY_FUNCTION_HPP_4SABI1FX

#include <concepts>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>

// a drop-in replacement for std::function that can hold move-only types
template <typename Signature>
class MoveOnlyFunction;

// try to mimic the std::function interface
template <typename Ret, typename... Args>
class MoveOnlyFunction<Ret(Args...)>
{
public:
    using Signature = Ret(Args...);

private:
    class FuncInterface
    {
    public:
        virtual ~FuncInterface()      = default;
        virtual Ret invoke(Args&&...) = 0;
        // virtual Ret invoke_const(Args&&...) const = 0;
    };

    template <typename Func>
    class FuncImpl : public FuncInterface
    {
    private:
        Func m_func;

    public:
        explicit FuncImpl(auto&& func)
            : m_func{ std::forward<Func>(func) }
        {
        }

        Ret invoke(Args&&... args)
            // requires std::invocable<Func&, Args...>
            override
        {
            return m_func(std::forward<Args>(args)...);
        }

        // Ret invoke_const(Args&&... args) const
        // requires std::invocable<const Func&, Args...>
        //     override
        // {
        //     return m_func(std::forward<Args>(args)...);
        // }
    };

private:
    std::unique_ptr<FuncInterface> m_func;

public:
    MoveOnlyFunction()                                   = default;
    MoveOnlyFunction(const MoveOnlyFunction&)            = delete;
    MoveOnlyFunction(MoveOnlyFunction&)                  = delete;
    MoveOnlyFunction& operator=(const MoveOnlyFunction&) = delete;
    MoveOnlyFunction& operator=(MoveOnlyFunction&)       = delete;

    template <typename Func>
        requires std::is_invocable_r_v<Ret, Func, Args...>
    MoveOnlyFunction(Func&& func)
        : m_func{ std::make_unique<FuncImpl<Func>>(std::forward<Func>(func)) }
    {
    }

    MoveOnlyFunction(MoveOnlyFunction&& other) noexcept
        : m_func{ std::move(other.m_func) }
    {
    }

    MoveOnlyFunction& operator=(MoveOnlyFunction&& other) noexcept
    {
        m_func = std::move(other.m_func);
        return *this;
    }

    Ret operator()(Args&&... args) const
    {
        if (!m_func) {
            throw std::bad_function_call{};
        }
        return m_func->invoke(std::forward<Args>(args)...);
    }

    // Ret operator()(Args&&... args)
    // // requires std::invocable<FuncInterface&, Args...>
    // {
    //     if (!m_func) {
    //         throw std::bad_function_call{};
    //     }
    //     return m_func->invoke(std::forward<Args>(args)...);
    // }

    // Ret operator()(Args&&... args) const
    // // requires std::invocable<const FuncInterface&, Args...>
    // {
    //     if (!m_func) {
    //         throw std::bad_function_call{};
    //     }
    //     return m_func->invoke_const(std::forward<Args>(args)...);
    // }
};

// TODO: add deduction guides

#endif /* end of include guard: MOVE_ONLY_FUNCTION_HPP_4SABI1FX */
