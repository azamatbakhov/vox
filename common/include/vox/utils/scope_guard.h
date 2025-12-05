#pragma once

#include "vox/utils/preprocessor.h"
#include <type_traits>

namespace vox::rt_detail
{
    template <typename Func> requires(std::is_invocable_v<Func>)
    struct ScopeGuardOnLeave
    {
        ScopeGuardOnLeave(Func callback) :
            m_callback(std::move(callback))
        {
        }

        ~ScopeGuardOnLeave()
        {
            std::invoke(m_callback);
        }
        
        Func m_callback;
    };
}

#define scope_on_leave \
    ::vox::rt_detail::ScopeGuardOnLeave ANONYMOUS_VAR(onScopeLeave__) = [&]
