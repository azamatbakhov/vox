#pragma once

#include <type_traits>
#include <typeinfo>

#include "vox/diag/assertion.h"
#include "vox/rtti/type_info.h"
#include "vox/utils/preprocessor.h"

namespace vox
{
    /**
     */
    struct VOX_ABSTRACT_TYPE IRttiObject
    {
        VOX_TYPEID(vox::IRttiObject);

        virtual ~IRttiObject() = default;
        virtual bool is(const rtti::TypeInfo&) const noexcept = 0;
        virtual void* as(const rtti::TypeInfo&) noexcept = 0;
        virtual const void* as(const rtti::TypeInfo&) const noexcept = 0;

        template <typename T>
        T as() const
            requires std::is_pointer_v<T>
        {
            using Interface = std::remove_pointer_t<T>;
            static_assert(std::is_const_v<Interface>, "Attempt to cast through constant instance. const T must be explicitly specified: use 'as<const T*>'");

            const void* const ptr = this->as(rtti::getTypeInfo<std::remove_const_t<Interface>>());
            return reinterpret_cast<T>(ptr);
        }

        template <typename T>
        T as()
            requires std::is_pointer_v<T>
        {
            using Interface = std::remove_pointer_t<T>;

            void* const ptr = this->as(rtti::getTypeInfo<std::remove_const_t<Interface>>());
            return reinterpret_cast<T>(ptr);
        }

        template <typename T>
        T as()
            requires std::is_reference_v<T>
        {
            using Interface = std::remove_reference_t<T>;
            void* const ptr = this->as(rtti::getTypeInfo<std::remove_const_t<Interface>>());
            VOX_ASSERT(ptr);

            return *reinterpret_cast<Interface*>(ptr);
        }

        template <typename T>
        T as() const
            requires std::is_reference_v<T>
        {
            using Interface = std::remove_reference_t<T>;
            static_assert(std::is_const_v<Interface>, "Attempt to cast through constant instance. const T must be explicitly specified: use 'as<const T&>'");

            const void* const ptr = this->as(rtti::getTypeInfo<std::remove_const_t<Interface>>());
            VOX_ASSERT(ptr);

            return *reinterpret_cast<Interface*>(ptr);
        }

        template <typename T, std::enable_if_t<!std::is_reference_v<T> && !std::is_pointer_v<T>, int> = 0>
        T as() const
        {
            constexpr bool NotPointerOrReference = !(std::is_reference_v<T> || std::is_pointer_v<T>);
            static_assert(NotPointerOrReference, "Type for 'as' must be pointer or reference: use 'as<T*>' or 'as<T&>'");
            return reinterpret_cast<T*>(nullptr);
        }

        template <typename T>
        bool is() const
        {
            static_assert(!(std::is_reference_v<T> || std::is_pointer_v<T> || std::is_const_v<T>), "Invalid requested type");

            if constexpr(rtti::HasTypeInfo<T>)
            {
                return this->is(rtti::getTypeInfo<T>());
            }
            else
            {
                return false;
            }
        }
    };

}
