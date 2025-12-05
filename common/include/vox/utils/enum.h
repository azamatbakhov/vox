#pragma once
#include <type_traits>

template <typename EnumType> requires(std::is_enum_v<EnumType>)
constexpr inline decltype(auto) operator+(EnumType value)
{
    return static_cast<std::underlying_type<EnumType>::type>(value);
}

template <typename EnumType> requires(std::is_enum_v<EnumType>)
constexpr inline EnumType operator|(EnumType const& lhs, EnumType const& rhs)
{
    return static_cast<EnumType>(+lhs | +rhs);
};
