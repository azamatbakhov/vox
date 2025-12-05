// Copyright 2024 N-GINN LLC. All rights reserved.
// Use of this source code is governed by a BSD-3 Clause license that can be found in the LICENSE file.


#include "./service_provider_impl.h"
#include "vox/thread/lock_guard.h"

namespace vox
{ 
    ServiceProviderImpl::ServiceProviderImpl()
    {
    }

    ServiceProviderImpl::~ServiceProviderImpl()
    {
        if (!m_isDisposed)
        {
        }
    }

    void ServiceProviderImpl::addServiceAccessorInternal(std::unique_ptr<ServiceAccessor>&& accessor)
    {
        VOX_ASSERT(accessor);
        if (!accessor)
        {
            return;
        }

        lock_(m_mutex);

        m_accessors.emplace_back(std::move(accessor));
    }


    void* ServiceProviderImpl::findInternal(const rtti::TypeInfo& type)
    {
        ServiceAccessor* accessor = nullptr;

        {
            shared_lock_(m_mutex);

            auto iter = m_instances.find(type);
            if (iter != m_instances.end())
            {
                return iter->second;
            }

            auto accessorIter = std::find_if(m_accessors.begin(), m_accessors.end(), [&type](const std::unique_ptr<ServiceAccessor>& accessor)
            {
                return accessor->hasApi(type);
            });
            accessor = accessorIter != m_accessors.end() ? accessorIter->get() : nullptr;
        }

        // getApi can also access to the service provider (through lazy service creation and service impl constructor's invocation)
        void* const api = accessor ? accessor->getApi(type) : nullptr;
        if (api)
        {
            lock_(m_mutex);
            m_instances.emplace(type, ServiceInstanceEntry{api, accessor});
        }

        return api;
    }

    void ServiceProviderImpl::findAllInternal(const rtti::TypeInfo& type,
        void (*callback)(void* instancePtr, void*), void* callbackData)
    {
        VOX_ASSERT(callback);
        if (!callback)
        {
            return;
        }

        // todo: use stack allocator
        std::vector<ServiceAccessor*> accessors;
        {
            shared_lock_(m_mutex);
            for (const std::unique_ptr<ServiceAccessor>& accessor : m_accessors)
            {
                if (accessor->hasApi(type))
                {
                    accessors.emplace_back(accessor.get());
                }
            }
        }

        for (auto& accessor : accessors)
        {
            // be aware: this is normal if even accessor::hasApi(type) return true,
            // but accessor::getApi(type) return nullptr (this can be when getApiMode == GetApiMode::DoNotCreate)
            void* const api = accessor->getApi(type);
            if (api)
            {
                callback(api, callbackData);
            }
        }
    }
     
    bool ServiceProviderImpl::hasApiInternal(const rtti::TypeInfo& type)
    {
        shared_lock_(m_mutex);

        return std::any_of(m_accessors.begin(), m_accessors.end(), [&type](auto& accessor)
        {
            return accessor->hasApi(type);
        });
    }

    //template<typename T>
    //T& ServiceProviderImpl::getInitializationInstance(T* instance)
    //{
    //    VOX_FATAL(instance);
    //    shared_lock_(m_mutex);

    //    if constexpr (std::is_same_v<IServiceInitialization, T>)
    //    {
    //        auto proxy = m_initializationProxy.find(instance);
    //        return proxy == m_initializationProxy.end() ? *instance : *proxy->second;
    //    }
    //    else
    //    {
    //        const IServiceInitialization* const serviceInit = instance->template as<const IServiceInitialization*>();
    //        if (auto proxyIter = m_initializationProxy.find(serviceInit); proxyIter != m_initializationProxy.end())
    //        {
    //            IServiceInitialization* const proxyServiceInit = proxyIter->second;
    //            T* const proxyTargetApi = proxyServiceInit->as<T*>();
    //            return proxyTargetApi ? *proxyTargetApi : *instance;
    //        }

    //        return *instance;
    //    }
    //}

//
//    async::Task<> ServiceProviderImpl::initServicesInternal(async::Task<> (*getTaskCallback)(IServiceInitialization&))
//    {
//        using namespace vox::async;
//
//        std::vector<IServiceInitialization*> services;
//        findAllInternal(rtti::getTypeInfo<IServiceInitialization>(), [](void* servicePtr, void* serviceCollection)
//        {
//            reinterpret_cast<decltype(services)*>(serviceCollection)->push_back(reinterpret_cast<IServiceInitialization*>(servicePtr));
//        }, &services, ServiceAccessor::GetApiMode::AllowLazyCreation);
//
//        auto [independentServices, orderedDependentServices] = makeInitOrderedServiceList(services);
//
//        {
//            std::vector<Task<>> independentInitializationTasks;
//            for (const ServiceEntry& entry : independentServices)
//            {
//                IServiceInitialization& serviceInstance = getInitializationInstance(entry.service);
//                if (auto task = getTaskCallback(serviceInstance); task && !task.isReady())
//                {
//                    independentInitializationTasks.emplace_back(std::move(task));
//                }
//            }
//
//            co_await whenAll(independentInitializationTasks);
//
//#ifdef VOX_ASSERT_ENABLED
//            for (auto& task : independentInitializationTasks)
//            {
//                if (task.isRejected())
//                {
//                    VOX_FAILURE(task.getError()->getDiagMessage().c_str());
//                }
//            }
//#endif
//        }
//
//        for (const ServiceEntry& serviceEntry : orderedDependentServices)
//        {
//            IServiceInitialization& serviceInstance = getInitializationInstance(serviceEntry.service);
//            if (Task<> task = getTaskCallback(serviceInstance); task)
//            {
//                co_await task;
//
//#ifdef VOX_ASSERT_ENABLED
//                if (task.isRejected())
//                {
//                    VOX_FAILURE(task.getError()->getDiagMessage().c_str());
//                }
//#endif
//            }
//        }
//    }
//

    //void ServiceProviderImpl::setInitializationProxy(const IServiceInitialization& source, IServiceInitialization* proxy)
    //{
    //    lock_(m_mutex);

    //    if (proxy)
    //    {
    //        VOX_ASSERT(!m_initializationProxy.contains(&source), "Proxy for source already set");
    //        m_initializationProxy[&source] = proxy;
    //    }
    //    else
    //    {
    //        m_initializationProxy.erase(&source);
    //    }
    //}

    //async::Task<> ServiceProviderImpl::preInitServices()
    //{
    //    return initServicesInternal([](IServiceInitialization& serviceInit)
    //    {
    //        return serviceInit.preInitService();
    //    });
    //}

    //async::Task<> ServiceProviderImpl::initServices()
    //{
    //    return initServicesInternal([](IServiceInitialization& serviceInit)
    //    {
    //        return serviceInit.initService();
    //    });
    //}

    //async::Task<> ServiceProviderImpl::shutdownServices()
    //{
    //    using namespace vox::async;

    //    constexpr auto NoLazyCreation = ServiceAccessor::GetApiMode::DoNotCreate;

    //    {
    //        lock_(m_mutex);
    //        m_isDisposed = true;
    //    }

    //    // Seems there is no need from this point to keep m_mutex locked any more:
    //    // m_accessors collection must not changed because shutdown

    //    // 1.1.creating ordered shutdown sequence: reverse order of initialization
    //    // 1.2 shutdown sequence consists of two sub-sequences:
    //    //      1) services with dependencies: must shutdown sequentially in specified order
    //    //      2) services without dependencies: must shutdown without blocking each other (i.e. run serviceShutdown without waiting previous)
    //    // 2. invoke service disposeAsync -> dispose, wait all dispose task

    //    {
    //        std::vector<IServiceShutdown*> unorderedShutdownSequence;

    //        for (const ServiceAccessor::Ptr& accessor : m_accessors)
    //        {
    //            if (void* const serviceShutdown = accessor->getApi(rtti::getTypeInfo<IServiceShutdown>(), NoLazyCreation))
    //            {
    //                unorderedShutdownSequence.push_back(reinterpret_cast<IServiceShutdown*>(serviceShutdown));
    //            }
    //        }

    //        auto [independentServices, orderedDependentServices] = makeShutdownOrderedServiceList(unorderedShutdownSequence);

    //        for (IServiceShutdown* const serviceShutdown : orderedDependentServices)
    //        {
    //            if (auto task = getInitializationInstance(serviceShutdown).shutdownService(); task)
    //            {
    //                co_await task;
    //            }
    //        }

    //        std::vector<Task<>> shutdownIndependentTasks;
    //        for (IServiceShutdown* const serviceShutdown : independentServices)
    //        {
    //            if (auto task = getInitializationInstance(serviceShutdown).shutdownService(); task && !task.isReady())
    //            {
    //                shutdownIndependentTasks.emplace_back(std::move(task));
    //            }
    //        }

    //        co_await whenAll(shutdownIndependentTasks);
    //    }

    //    {
    //        std::vector<Task<>> disposeTasks;

    //        {
    //            for (const ServiceAccessor::Ptr& accessor : m_accessors)
    //            {
    //                // invoke disposeAsync first.
    //                // same class can provide both IAsyncDisposable and IDisposable api,
    //                // so first async version must be called and in this case dispose can do nothing (because if async variant called).
    //                if (void* const asyncDisposable = accessor->getApi(rtti::getTypeInfo<IAsyncDisposable>(), NoLazyCreation))
    //                {
    //                    Task<> task = reinterpret_cast<IAsyncDisposable*>(asyncDisposable)->disposeAsync();
    //                    if (task && !task.isReady())
    //                    {
    //                        disposeTasks.push_back(std::move(task));
    //                    }
    //                }

    //                if (void* const disposable = accessor->getApi(rtti::getTypeInfo<IDisposable>(), NoLazyCreation))
    //                {
    //                    reinterpret_cast<IDisposable*>(disposable)->dispose();
    //                }
    //            }
    //        }

    //        co_await whenAll(disposeTasks);
    //    }
    //}

    namespace
    {
        std::unique_ptr<ServiceProvider>& getServiceProviderInstanceRef()
        {
            static std::unique_ptr<ServiceProvider> s_serviceProvider;
            return (s_serviceProvider);
        }
    }  // namespace

    std::unique_ptr<ServiceProvider> createServiceProvider()
    {
        return std::make_unique<ServiceProviderImpl>();
    }

    void setDefaultServiceProvider(std::unique_ptr<ServiceProvider>&& provider)
    {
        VOX_FATAL(!provider || !getServiceProviderInstanceRef(), "Service provider already set");
        getServiceProviderInstanceRef() = std::move(provider);
    }

    bool hasServiceProvider()
    {
        return static_cast<bool>(getServiceProviderInstanceRef());
    }

    ServiceProvider& getServiceProvider()
    {
        auto& instance = getServiceProviderInstanceRef();
        VOX_FATAL(instance);
        return *instance;
    }

}  // namespace vox
