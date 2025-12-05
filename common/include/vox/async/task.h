#pragma once

#include <optional>
#include <coroutine>
#include <tuple>


namespace vox::async_detail
{
    template <typename T>
    struct TaskPromise;
}
    
namespace vox::async
{
    template<typename T>
    struct Task : std::coroutine_handle<async_detail::TaskPromise<T>>
    {
        using promise_type = async_detail::TaskPromise<T>;
    };

}

namespace vox::async_detail
{
    template <typename T>
    struct TaskAwaiter
    {
        bool await_ready() const noexcept
        {
            return false;
        }

        template <typename Promise>
        void await_suspend(std::coroutine_handle<Promise> coroutine) noexcept
        {
            
        }

        T await_resume() const
        {
            return {};
        }
    };

    template <typename T>
    struct TaskPromise
    {
        async::Task<T> get_return_object()
        {
            return {};
        }

        std::suspend_never initial_suspend() const noexcept
        {
            return {};
        }

        std::suspend_never final_suspend() const noexcept
        {
            return {};
        }

        void unhandled_exception() noexcept
        {
        }
    };

}