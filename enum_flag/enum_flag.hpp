#ifndef ENUM_FLAG_HPP_GLA1CZ3V
#define ENUM_FLAG_HPP_GLA1CZ3V

#include <concepts>
#include <type_traits>

template <typename Enum>
concept Enumeration = std::is_enum_v<Enum>;

/*
 * @brief A class that allows you to use enum as flags.
 * The enum must have a value that is a power of 2 in order for this to work.
 * Example:
 *
 * @code{cpp}
 *      enum class FlagBit {
 *          Flag1 = 1 << 0,
 *          Flag2 = 1 << 1,
 *          Flag3 = 1 << 2,
 *      };
 *
 *      class Flag : EnumFlagBase<FlagBit, 3>
 *      {
 *      public:
 *          Flag(const std::same_as<FlagBit> auto&... e)
 *              : EnumFlagBase{ e... }
 *          {
 *          }
 *
 *          // ...
 *
 *          // define other methods
 *          void print() { std::cout << ord() << '\n'; }
 *      };
 * @endcode
 */
template <Enumeration Enumeration, std::size_t EnumerationSize>
class EnumFlagBase
{
public:
    using Enum     = Enumeration;
    using EnumBase = std::underlying_type_t<Enumeration>;

    static constexpr EnumBase ALL  = (1 << EnumerationSize) - 1;
    static constexpr EnumBase NONE = 0;

    static constexpr std::size_t len() { return EnumerationSize; }
    static constexpr std::size_t size() { return EnumerationSize; }

    EnumFlagBase(const std::same_as<Enum> auto&... e)
        : m_value{ static_cast<EnumBase>((0 | ... | toUnderlying(e))) }
    {
    }

    void set(const std::same_as<Enum> auto&... e)
        requires(sizeof...(e) > 0)
    {
        m_value |= (NONE | ... | toUnderlying(e));
    }

    void setAll() { m_value = ALL; }

    void setAllExcept(const std::same_as<Enum> auto&... e)
        requires(sizeof...(e) > 0)
    {
        m_value = ALL;
        unset(e...);
    }

    void unset(const std::same_as<Enum> auto&... e)
        requires(sizeof...(e) > 0)
    {
        m_value &= ~(NONE | ... | toUnderlying(e));
    }

    void unsetAll() { m_value = NONE; }

    void unsetAllExcept(const std::same_as<Enum> auto&... e)
        requires(sizeof...(e) > 0)
    {
        m_value = NONE;
        set(e...);
    }

    void toggle(const std::same_as<Enum> auto&... e)
        requires(sizeof...(e) > 0)
    {
        m_value ^= (NONE | ... | toUnderlying(e));
    }

    void toggleAll() { m_value ^= ALL; }

    void toggleAllExcept(const std::same_as<Enum> auto&... e)
        requires(sizeof...(e) > 0)
    {
        m_value ^= ALL;
        toggle(e...);
    }

    void setToValue(bool value, const std::same_as<Enum> auto&... e)
        requires(sizeof...(e) > 0)
    {
        if (value) {
            set(e...);
        } else {
            unset(e...);
        }
    }

    void setAllToValue(bool value)
    {
        if (value) {
            setAll();
        } else {
            unsetAll();
        }
    }

    // test if all of the specified flags are set.
    // use testAll() and testNone() for checking if all or none of the flags are set.
    bool test(const std::same_as<Enum> auto&... e) const
        requires(sizeof...(e) > 0)
    {
        auto flags{ static_cast<EnumBase>((NONE | ... | toUnderlying(e))) };
        return (m_value & flags) == flags;
    }

    // test if any of the specified flags are set.
    bool testSome(const std::same_as<Enum> auto&... e) const
        requires(sizeof...(e) > 0)
    {
        auto flags{ static_cast<EnumBase>((NONE | ... | toUnderlying(e))) };
        return (m_value & flags) != NONE;
    }

    bool testAny() const { return m_value != NONE; }
    bool testNone() const { return m_value == NONE; }
    bool testAll() const { return m_value == ALL; }

    bool testAllExcept(const std::same_as<Enum> auto&... e) const
        requires(sizeof...(e) > 0)
    {
        auto flags{ static_cast<EnumBase>((NONE | ... | toUnderlying(e))) };
        return (m_value & ~flags) == NONE;
    }

    void reset() { m_value = NONE; }
    bool isValid() const { return m_value <= ALL; }

    void removeInvalidBits() { m_value &= ALL; }

    // get the underlying value. proceed with caution as modifying the value directly can lead to invalid states (bits
    // outside the domain being set). use ord() if you want to get the raw value representation as copy.
    EnumBase& base() { return m_value; }

    EnumBase ord() const { return m_value; }

protected:
    static constexpr EnumBase toUnderlying(Enum e) { return static_cast<EnumBase>(e); }
    EnumBase                  m_value;
};

#endif /* end of include guard: ENUM_FLAG_HPP_GLA1CZ3V */
