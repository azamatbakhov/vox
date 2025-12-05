#pragma once
#include <cstddef>

#include "vox/rtti/rtti_object.h"
#include "vox/rtti/rtti_utils.h"

#define VOX_IMPLEMENT_RTTI_OBJECT                                             \
                                                                              \
public:                                                                       \
    using ::vox::IRttiObject::is;                                             \
    using ::vox::IRttiObject::as;                                             \
                                                                              \
    bool is(const ::vox::rtti::TypeInfo& type) const noexcept override        \
    {                                                                         \
        return false;/*::vox::rtti::runtimeIs<decltype(*this)>(type);*/       \
    }                                                                         \
                                                                              \
    void* as(const ::vox::rtti::TypeInfo& type) noexcept override             \
    {                                                                         \
        return nullptr/*::vox::rtti::runtimeCast(*this, type)*/;              \
    }                                                                         \
                                                                              \
    const void* as(const ::vox::rtti::TypeInfo& type) const noexcept override \
    {                                                                         \
        return nullptr/*::vox::rtti::runtimeCast(*this, type)*/;              \
    }

#define VOX_RTTI_CLASS(ClassImpl, ...) \
    VOX_TYPEID(ClassImpl)              \
                                       \
    VOX_IMPLEMENT_RTTI_OBJECT

#define VOX_CLASS(ClassImpl, ...) \
    VOX_RTTI_CLASS(ClassImpl, __VA_ARGS__)  \

#define VOX_INTERFACE(TypeName, ...) \
    VOX_TYPEID(TypeName)