#pragma once

#include <type_traits>

namespace vox
{

template <typename T> requires(std::is_enum_v<T>)
class TypedMask
{
public:
    using EnumType = T;
    using ValueType = std::underlying_type_t<T>;

    TypedMask() = default;

    constexpr TypedMask(T flag) :
        m_value(static_cast<ValueType>(flag))
    {
    }

    template <typename... U>
    requires(sizeof...(U) > 0 && (std::is_same_v<T, U> && ...))
    constexpr TypedMask(T value, U... values) :
        m_value(static_cast<ValueType>(value) | (static_cast<ValueType>(values) | ...))
    {
    }

    TypedMask(const TypedMask&) = default;

    template <typename... U>
    requires((std::is_same_v<T, U> && ...))
    TypedMask<T>& set(U... flags)
    {
        static_assert(sizeof...(flags) > 0);
        const ValueType combinedFlag = (static_cast<ValueType>(flags) | ...);
        m_value |= combinedFlag;
        return *this;
    }

    TypedMask<T>& set(TypedMask<T> flags)
    {
        m_value |= flags.m_value;
        return *this;
    }
    template <typename... U>
    requires((std::is_same_v<T, U> && ...))
    TypedMask<T>& unset(U... flags)
    {
        static_assert(sizeof...(flags) > 0);

        const ValueType combinedFlag = (static_cast<ValueType>(flags) | ...);

        m_value &= ~combinedFlag;
        return *this;
    }

    TypedMask<T>& unset(TypedMask<T> flags)
    {
        m_value &= ~flags.m_value;
        return *this;
    }

    constexpr bool has(TypedMask<T> flags) const
    {
        return (m_value & flags.m_value) == flags.m_value;
    }

    constexpr bool hasAny(TypedMask<T> flags) const
    {
        return (m_value & flags.m_value) != 0;
    }

    template <typename... U>
    requires((std::is_same_v<T, U> && ...))
    constexpr bool has(U... flags) const
    {
        static_assert(sizeof...(flags) > 0);

        const ValueType combinedFlag = (static_cast<ValueType>(flags) | ...);
        return (m_value & combinedFlag) == combinedFlag;
    }

    template <typename... U>
    requires((std::is_same_v<T, U> && ...))
    constexpr bool hasAny(U... flags) const
    {
        static_assert(sizeof...(flags) > 0);

        const ValueType combinedFlag = (static_cast<ValueType>(flags) | ...);
        return (m_value & combinedFlag) != 0;
    }

    constexpr bool isEmpty() const
    {
        return m_value == 0;
    }

    void clear()
    {
        m_value = 0;
    }

    constexpr operator ValueType() const
    {
        return m_value;
    }

private:
    ValueType m_value = 0;

    friend constexpr TypedMask<T> operator|(TypedMask<T> value, T flag)
    {
        return TypedMask<T>{value}.set(flag);
    }

    friend TypedMask<T>& operator|=(TypedMask<T>& value, T flag)
    {
        return value.set(flag);
    }

    friend TypedMask<T>& operator|=(TypedMask<T>& value, TypedMask<T> flag)
    {
        return value.set(flag);
    }

    friend TypedMask<T> operator+(TypedMask<T> value, T flag)
    {
        return TypedMask<T>{value}.set(flag);
    }

    friend TypedMask<T>& operator+=(TypedMask<T>& value, T flag)
    {
        return value.set(flag);
    }

    friend TypedMask<T>& operator-=(TypedMask<T>& value, T flag)
    {
        return value.unset(flag);
    }

    friend TypedMask<T>& operator-=(TypedMask<T>& value, std::initializer_list<T> flags)
    {
        for (T flag : flags)
        {
            value.m_value &= ~static_cast<ValueType>(flag);
        }

        return value;
    }

    friend TypedMask<T> operator-(TypedMask<T> value, T flag)
    {
        return TypedMask<T>{value}.unset(flag);
    }

    friend bool operator&&(TypedMask<T> value, T flag)
    {
        return value.has(flag);
    }

    friend bool operator&&(TypedMask<T> value, TypedMask<T> flag)
    {
        return value.has(flag);
    }

    friend constexpr bool operator==(TypedMask<T> value, T flag)
    {
        return value.m_value == static_cast<ValueType>(flag);
    }

    friend constexpr bool operator==(TypedMask<T> value1, TypedMask<T> value2)
    {
        return value1.m_value == value2.m_value;
    }
};

}
