// Copyright 2024 N-GINN LLC. All rights reserved.
// Use of this source code is governed by a BSD-3 Clause license that can be found in the LICENSE file.


#pragma once

#include <list>
#include <unordered_map>

#include "vox/rtti/rtti_impl.h"
#include "vox/service/internal/service_provider_initialization.h"
#include "vox/service/service.h"
#include "vox/service/service_provider.h"
#include "vox/utils/scope_guard.h"

namespace vox
{
    class ServiceProviderImpl final : public ServiceProvider,
                                      public core_detail::IServiceProviderInitialization
    {
        VOX_RTTI_CLASS(vox::ServiceProviderImpl, ServiceProvider, core_detail::IServiceProviderInitialization)
    
    public:
        ServiceProviderImpl();

        ~ServiceProviderImpl();

    private:
        struct ServiceInstanceEntry
        {
            void* const serviceInstance;
            ServiceAccessor* const accessor;

            ServiceInstanceEntry(void* inServiceInstance, ServiceAccessor* inAccessor) :
                serviceInstance(inServiceInstance),
                accessor(inAccessor)
            {
            }

            operator void*() const
            {
                return serviceInstance;
            }
        };

        void* findInternal(const rtti::TypeInfo&) override;

        void findAllInternal(const rtti::TypeInfo&, void (*)(void* instancePtr, void*), void*) override;

        void addServiceAccessorInternal(std::unique_ptr<ServiceAccessor>&&) override;

        bool hasApiInternal(const rtti::TypeInfo&) override;

        //void setInitializationProxy(const IServiceInitialization& source, IServiceInitialization* proxy) override;

        /*async::Task<> preInitServices() override;

        async::Task<> initServices() override;

        async::Task<> shutdownServices() override;

        async::Task<> initServicesInternal(async::Task<> (*)(IServiceInitialization&));*/

      /*  template<typename T>
        T& getInitializationInstance(T* instance);*/

        std::list<std::unique_ptr<ServiceAccessor>> m_accessors;
        std::unordered_map<rtti::TypeIndex, ServiceInstanceEntry> m_instances;


        std::shared_mutex m_mutex;
        bool m_isDisposed = false;
    };
}  // namespace vox
