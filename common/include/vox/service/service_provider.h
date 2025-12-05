#pragma once

#include <memory>
#include <mutex>
#include <type_traits>

#include "vox/diag/assertion.h"
#include "vox/rtti/rtti_impl.h"
#include "vox/rtti/rtti_object.h"
#include "vox/rtti/rtti_utils.h"
#include "vox/rtti/type_info.h"
#include "vox/utils/scope_guard.h"

namespace vox
{
    struct VOX_ABSTRACT_TYPE ServiceAccessor
    {
        virtual ~ServiceAccessor() = default;

        virtual void* getApi(const rtti::TypeInfo&) = 0;

        virtual bool hasApi(const rtti::TypeInfo&) = 0;
    };
}

namespace vox::core_detail
{
    template <template <typename, typename...> class SmartPtrT = std::unique_ptr>
    class RttiServiceAccessor final : public ServiceAccessor
    {
    public:
        template <typename T>
        RttiServiceAccessor(SmartPtrT<T> instance) :
            m_instance(std::move(instance))
        {
        }

        void* getApi(const rtti::TypeInfo& type) override
        {
            VOX_FATAL(m_instance);
            return m_instance->as(type);
        }

        bool hasApi(const rtti::TypeInfo& type) override
        {
            VOX_FATAL(m_instance);
            return m_instance->is(type);
        }

    private:
        const SmartPtrT<IRttiObject> m_instance;
    };
}

namespace vox
{
    struct VOX_ABSTRACT_TYPE ServiceProvider : virtual IRttiObject
    {
        VOX_INTERFACE(ServiceProvider, IRttiObject)

        virtual ~ServiceProvider() = default;

        template <rtti::WithTypeInfo T>
        bool has();

        template <rtti::WithTypeInfo T>
        T& get();

        template <rtti::WithTypeInfo T>
        T* find();

        template <rtti::WithTypeInfo T>
        [[nodiscard]]
        std::vector<T*> getAll();

        template <rtti::WithTypeInfo T, typename Predicate>
        T* findIf(Predicate);

        /**
            @brief register existing service instance
        */
        template <rtti::WithTypeInfo T>
        void addService(std::unique_ptr<T>&&);
    protected:
        virtual void* findInternal(const rtti::TypeInfo&) = 0;

        virtual void findAllInternal(const rtti::TypeInfo&, void (*)(void* instancePtr, void*), void*) = 0;

        virtual void addServiceAccessorInternal(std::unique_ptr<ServiceAccessor>&&) = 0;

        virtual bool hasApiInternal(const rtti::TypeInfo&) = 0;
    };

    template <rtti::WithTypeInfo T>
    bool ServiceProvider::has()
    {
        return hasApiInternal(rtti::getTypeInfo<T>());
    }

    template <rtti::WithTypeInfo T>
    T& ServiceProvider::get()
    {
        void* const service = findInternal(rtti::getTypeInfo<T>());
        VOX_ASSERT(service, "Service ({}) does not exists", rtti::getTypeInfo<T>().getTypeName());
        return *reinterpret_cast<T*>(service);
    }

    template <rtti::WithTypeInfo T>
    T* ServiceProvider::find()
    {
        void* const service = findInternal(rtti::getTypeInfo<T>());
        return service ? reinterpret_cast<T*>(service) : nullptr;
    }

    template <rtti::WithTypeInfo T>
    std::vector<T*> ServiceProvider::getAll()
    {
        std::vector<T*> services;
        findAllInternal(rtti::getTypeInfo<T>(), [](void* ptr, void* data)
        {
            T* const instance = reinterpret_cast<T*>(ptr);
            auto& container = *reinterpret_cast<decltype(services)*>(data);

            container.push_back(instance);
        }, &services);

        return services;
    }

    template <rtti::WithTypeInfo T, typename Predicate>
    T* ServiceProvider::findIf(Predicate predicate)
    {
        static_assert(std::is_invocable_r_v<bool, Predicate, T&>,
            "Invalid predicate callback: expected (T&) -> bool");

        for (T* const instance : getAll<std::remove_const_t<T>>())
        {
            if (predicate(*instance))
            {
                return instance;
            }
        }

        return nullptr;
    }

    template <rtti::WithTypeInfo T>
    void ServiceProvider::addService(std::unique_ptr<T>&& instance)
    {
        VOX_ASSERT(instance);
        if (!instance)
        {
            return;
        }

        using Accessor = core_detail::RttiServiceAccessor<std::unique_ptr>;
        addServiceAccessorInternal(std::make_unique<Accessor>(std::move(instance)));
    }

    std::unique_ptr<ServiceProvider> createServiceProvider();

    void setDefaultServiceProvider(std::unique_ptr<ServiceProvider>&&);

    bool hasServiceProvider();

    ServiceProvider& getServiceProvider();
}