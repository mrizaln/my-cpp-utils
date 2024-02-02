#ifndef STRINGIFIED_ENUM_HPP_XNNATRPL
#define STRINGIFIED_ENUM_HPP_XNNATRPL

#include <map>
#include <string>

// clang-format off
#define STRINGIFIED_ENUM__DETAIL_FIELD_EXPANDER(Name)                Name
#define STRINGIFIED_ENUM__DETAIL_MAP_EXPANDER(Name)                   { Name, #Name }
#define STRINGIFIED_ENUM_STRING__DETAIL_FIELD_EXPANDER(Name, String) Name
#define STRINGIFIED_ENUM_STRING__DETAIL_MAP_EXPANDER(Name, String)   { Name, String }
// clang-format on

// just simply enum that has string representation, not used as flags.
#define STRINGIFIED_ENUM__DETAIL_IMPL(Name, BaseType, FIELDS, FIELD_EXPANDER, MAP_EXPANDER)                             \
    class Name                                                                                                         \
    {                                                                                                                  \
    public:                                                                                                            \
        static constexpr const char* const s_className = #Name;                                                        \
                                                                                                                       \
        enum Name##Enum : BaseType{ FIELDS(FIELD_EXPANDER) };                                                          \
        using Enum_type = Name##Enum;                                                                                  \
                                                                                                                       \
        static inline const std::map<Enum_type, std::string> s_map{ FIELDS(MAP_EXPANDER) };                            \
                                                                                                                       \
    private:                                                                                                           \
        Enum_type m_value;                                                                                             \
                                                                                                                       \
    public:                                                                                                            \
        Name(Enum_type value)                                                                                          \
            : m_value{ value }                                                                                         \
        {                                                                                                              \
        }                                                                                                              \
                                                                                                                       \
        operator Enum_type() const { return m_value; }                                                                 \
                                                                                                                       \
        const std::string& str() const { return s_map.at(m_value); }                                                   \
                                                                                                                       \
        Enum_type value() const { return m_value; }                                                                    \
    }

/*
 * Example usage:
 * ```
 *      #define FIELDS(M) \
 *          M(Flag1),     \
 *          M(Flag2),     \
 *          M(Flag3),     \
 *          M(Flag4)
 *
 *      STRINGIFIED_ENUM(EnumName, int, FIELDS);
 *      #undef FIELDS
 * ```
 */
#define STRINGIFIED_ENUM(Name, BaseType, FIELDS)                                                                       \
    STRINGIFIED_ENUM__DETAIL_IMPL(                                                                                      \
        Name, BaseType, FIELDS, STRINGIFIED_ENUM__DETAIL_FIELD_EXPANDER, STRINGIFIED_ENUM__DETAIL_MAP_EXPANDER         \
    )

/*
 * Example usage:
 * ```
 *      #define FIELDS(M) \
 *          M(Flag1, "Flag1Name"),     \
 *          M(Flag2, "Flag2Name"),     \
 *          M(Flag3, "Flag3Name"),     \
 *          M(Flag4, "Flag4Name")
 *
 *      STRINGIFIED_ENUM_STRING(EnumName, int, FIELDS);
 *      #undef FIELDS
 * ```
 */
#define STRINGIFIED_ENUM_STRING(Name, BaseType, FIELDS)                                                                \
    STRINGIFIED_ENUM__DETAIL_IMPL(                                                                                      \
        Name,                                                                                                          \
        BaseType,                                                                                                      \
        FIELDS,                                                                                                        \
        STRINGIFIED_ENUM_STRING__DETAIL_FIELD_EXPANDER,                                                                \
        STRINGIFIED_ENUM_STRING__DETAIL_MAP_EXPANDER                                                                   \
    )

#endif /* end of include guard: STRINGIFIED_ENUM_HPP_XNNATRPL */
