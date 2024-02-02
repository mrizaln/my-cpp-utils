#ifndef DEFER_HPP_3SRMJBZD
#define DEFER_HPP_3SRMJBZD

#ifdef defer
#    error "defer macro already defined"
#else

namespace defer_detail
{
    struct Deferrer_dummy
    {
    };

    template <typename F>
    struct Deferrer
    {
        F f;
        ~Deferrer() { f(); }
    };

    template <typename F>
    Deferrer<F> operator*(Deferrer_dummy, F&& f)
    {
        return { f };
    }
}

#    define DEFER__DETAIL_CONCAT(PREFIX, NUM) PREFIX##NUM
#    define DEFER__DETAIL_VAR_NAME(NUM)       DEFER__DETAIL_CONCAT(zz_defer_number_, NUM)
#    define defer                             [[maybe_unused]] auto DEFER__DETAIL_VAR_NAME(__COUNTER__) = defer_detail::Deferrer_dummy{}* [&]()

#endif    // defer

#endif /* end of include guard: DEFER_HPP_3SRMJBZD */
