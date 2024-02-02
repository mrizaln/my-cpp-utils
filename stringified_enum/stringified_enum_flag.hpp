#ifndef STRINGIFIED_ENUM_FLAG_HPP_HY6RR7F8
#define STRINGIFIED_ENUM_FLAG_HPP_HY6RR7F8

#include <bitset>
#include <concepts>
#include <format>
#include <map>
#include <optional>
#include <ostream>
#include <string>
#include <utility>

// clang-format off
#define STRINGIFIED_ENUM_FLAG__DETAIL_FIELD_EXPANDER(Name)                Name = 1 << (__COUNTER__ - _counter_start_ - 1)
#define STRINGIFIED_ENUM_FLAG__DETAIL_MAP_EXPANDER(Name)                  { (Base_type)Name, #Name }
#define STRINGIFIED_ENUM_FLAG_STRING__DETAIL_FIELD_EXPANDER(Name, String) Name = 1 << (__COUNTER__ - _counter_start_ - 1)
#define STRINGIFIED_ENUM_FLAG_STRING__DETAIL_MAP_EXPANDER(Name, String)   { (Base_type)Name, String }
// clang-format on

#define STRINGIFIED_ENUM_FLAG__DETAIL_IMPL(Name, BaseType, FIELDS, FIELD_EXPANDER, MAP_EXPANDER)                       \
    class Name                                                                                                         \
    {                                                                                                                  \
    private:                                                                                                           \
        static inline constexpr std::size_t _counter_start_{ __COUNTER__ };                                            \
                                                                                                                       \
    public:                                                                                                            \
        using Base_type = BaseType;                                                                                    \
                                                                                                                       \
        static inline constexpr const char* const s_className = #Name;                                                 \
                                                                                                                       \
        enum Enum : Base_type                                                                                          \
        {                                                                                                              \
            NONE = 0,                                                                                                  \
            FIELDS(FIELD_EXPANDER),                                                                                    \
            ALL = (1 << (__COUNTER__ - _counter_start_ - 1)) - 1                                                       \
        };                                                                                                             \
                                                                                                                       \
        static inline const std::map<Base_type, std::string> s_enums{                                                  \
            FIELDS(MAP_EXPANDER),                                                                                      \
        };                                                                                                             \
                                                                                                                       \
        static std::optional<Enum> fromString(const std::string& s)                                                    \
        {                                                                                                              \
            for (const auto& [val, name] : s_enums) {                                                                  \
                if (name == s) {                                                                                       \
                    return (Enum)val;                                                                                  \
                }                                                                                                      \
            }                                                                                                          \
            return {};                                                                                                 \
        }                                                                                                              \
                                                                                                                       \
    private:                                                                                                           \
        Base_type m_flags;                                                                                             \
                                                                                                                       \
    public:                                                                                                            \
        Name(std::same_as<Enum> auto&&... e)                                                                           \
            : m_flags{ static_cast<Base_type>((0 | ... | e)) }                                                         \
        {                                                                                                              \
        }                                                                                                              \
                                                                                                                       \
        Name(const Name&)            = default;                                                                        \
        Name(Name&&)                 = default;                                                                        \
        Name& operator=(const Name&) = default;                                                                        \
        Name& operator=(Name&&)      = default;                                                                        \
                                                                                                                       \
    private:                                                                                                           \
        Name(Base_type flags)                                                                                          \
            : m_flags{ flags }                                                                                         \
        {                                                                                                              \
        }                                                                                                              \
                                                                                                                       \
    public:                                                                                                            \
        auto& set(std::same_as<Enum> auto&&... e)                                                                      \
            requires(sizeof...(e) > 0)                                                                                 \
        {                                                                                                              \
            m_flags |= (NONE | ... | e);                                                                               \
            return *this;                                                                                              \
        }                                                                                                              \
                                                                                                                       \
        auto& unset(std::same_as<Enum> auto&&... e)                                                                    \
            requires(sizeof...(e) > 0)                                                                                 \
        {                                                                                                              \
            m_flags &= ~(NONE | ... | e);                                                                              \
            return *this;                                                                                              \
        }                                                                                                              \
                                                                                                                       \
        auto& toggle(std::same_as<Enum> auto&&... e)                                                                   \
            requires(sizeof...(e) > 0)                                                                                 \
        {                                                                                                              \
            m_flags ^= (NONE | ... | e);                                                                               \
            return *this;                                                                                              \
        }                                                                                                              \
                                                                                                                       \
        auto& setToValue(bool value, std::same_as<Enum> auto&&... e)                                                   \
            requires(sizeof...(e) > 0)                                                                                 \
        {                                                                                                              \
            if (value) {                                                                                               \
                return set(std::forward<Enum>(e)...);                                                                  \
            } else {                                                                                                   \
                return unset(std::forward<Enum>(e)...);                                                                \
            }                                                                                                          \
        }                                                                                                              \
                                                                                                                       \
        /* test if all of the specified flags are set. */                                                              \
        /* pass ALL to check all flags are set or NONE to check none of the flags are set. */                          \
        bool test(std::same_as<Enum> auto&&... e) const                                                                \
            requires(sizeof...(e) > 0)                                                                                 \
        {                                                                                                              \
            auto flags{ static_cast<Base_type>((NONE | ... | e)) };                                                    \
            if (flags == NONE) return m_flags == NONE;                                                                 \
            else return (m_flags & flags) == flags;                                                                    \
        }                                                                                                              \
                                                                                                                       \
        /* test if any of the specified flags are set. */                                                              \
        /* if the combined value of the flags is NONE, then the test() function will be used instead. */               \
        bool testSome(std::same_as<Enum> auto&&... e) const                                                            \
            requires(sizeof...(e) > 0)                                                                                 \
        {                                                                                                              \
            auto flags{ static_cast<Base_type>((NONE | ... | e)) };                                                    \
            if (flags == NONE) return test(static_cast<Enum>(flags));                                                  \
            else return (m_flags & flags) != NONE;                                                                     \
        }                                                                                                              \
                                                                                                                       \
        auto& reset()                                                                                                  \
        {                                                                                                              \
            m_flags = NONE;                                                                                            \
            return *this;                                                                                              \
        }                                                                                                              \
                                                                                                                       \
        const std::string str(bool withValues = false) const                                                           \
        {                                                                                                              \
            constexpr std::size_t maxFlags{ sizeof(Base_type) * 8 };                                                   \
            std::size_t           usedFlags{ size() };                                                                 \
                                                                                                                       \
            std::string binaryRepresentation{ std::bitset<8 * sizeof(Base_type)>{ ord() }.to_string() };               \
            std::string result{ std::format(                                                                           \
                "{0}[{1}]: ",                                                                                          \
                binaryRepresentation.substr(0, maxFlags - usedFlags),                                                  \
                binaryRepresentation.substr(maxFlags - usedFlags)                                                      \
            ) };                                                                                                       \
                                                                                                                       \
            if (ord() > ALL || ord() < 0) return result + "INVALID";                                                   \
                                                                                                                       \
            if (m_flags == NONE) {                                                                                     \
                if (withValues) return result + std::format("{} [{}]", "NONE", ord());                                 \
                else return result + std::format("{}", "NONE");                                                        \
            } else if (m_flags == ALL) {                                                                               \
                if (withValues) return result + std::format("{} [{}]", "ALL", ord());                                  \
                else return result + std::format("{}", "ALL");                                                         \
            }                                                                                                          \
                                                                                                                       \
            bool first{ true };                                                                                        \
            for (const auto& [val, name] : s_enums) {                                                                  \
                if (val == NONE || val == ALL) {                                                                       \
                    continue;                                                                                          \
                } else if (m_flags & val) {                                                                            \
                    if (!first) result.append(" | ");                                                                  \
                    first = false;                                                                                     \
                                                                                                                       \
                    if (withValues) result.append(std::format("{} [{}]", name, (std::size_t)val));                     \
                    else result.append(std::format("{}", name, (std::size_t)val));                                     \
                }                                                                                                      \
            }                                                                                                          \
                                                                                                                       \
            return result;                                                                                             \
        }                                                                                                              \
                                                                                                                       \
        Base_type&  base() { return m_flags; }                                                                         \
        std::size_t ord() const { return (std::size_t)m_flags; }                                                       \
                                                                                                                       \
        static std::size_t len() { return s_enums.size(); }                                                            \
        static std::size_t size() { return len(); }                                                                    \
                                                                                                                       \
        operator const Base_type&() const { return m_flags; }                                                          \
        operator const std::string() const { return this->str(); }                                                     \
                                                                                                                       \
        bool                 operator==(const Name& other) { return m_flags == other.m_flags; }                        \
        friend std::ostream& operator<<(std::ostream& out, const Name& e) { return out << e.str(true); }               \
    }

/*
 * Example usage:
 * ```
 *      #define FIELDS(M) \
 *          M(Flag1),     \
 *          M(Flag2),     \
 *          M(Flag3),     \
 *          M(Flag4),     \
 *
 *      STRINGIFIED_ENUM_FLAG(EnumFlagName, int, FIELDS);
 *      #undef FIELDS
 * ```
 */
#define STRINGIFIED_ENUM_FLAG(EnumFlagName, BaseType, FIELDS)                                                          \
    STRINGIFIED_ENUM_FLAG__DETAIL_IMPL(                                                                                \
        EnumFlagName,                                                                                                  \
        BaseType,                                                                                                      \
        FIELDS,                                                                                                        \
        STRINGIFIED_ENUM_FLAG__DETAIL_FIELD_EXPANDER,                                                                  \
        STRINGIFIED_ENUM_FLAG__DETAIL_MAP_EXPANDER                                                                     \
    );

/*
 * Example usage:
 * ```
 *      #define FIELDS(M)          \
 *          M(Flag1, "Flag1Name"), \
 *          M(Flag2, "Flag2Name"), \
 *          M(Flag3, "Flag3Name"), \
 *          M(Flag4, "Flag4Name")
 *
 *      STRINGIFIED_ENUM_FLAG(EnumFlagName, int, FIELDS);
 *      #undef FIELDS
 * ```
 */

#define STRINGIFIED_ENUM_FLAG_STRING(EnumFlagName, BaseType, FIELD)                                                    \
    STRINGIFIED_ENUM_FLAG__DETAIL_IMPL(                                                                                \
        EnumFlagName,                                                                                                  \
        BaseType,                                                                                                      \
        FIELDS,                                                                                                        \
        STRINGIFIED_ENUM_FLAG_STRING__DETAIL_FIELD_EXPANDER,                                                           \
        STRINGIFIED_ENUM_FLAG_STRING__DETAIL_MAP_EXPANDER                                                              \
    )

#endif /* end of include guard: STRINGIFIED_ENUM_FLAG_HPP_HY6RR7F8 */
