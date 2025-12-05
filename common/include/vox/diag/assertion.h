#pragma once

#include <format>
#include <string_view>
#include <type_traits>

#include "vox/debug/debugger.h"
#include "vox/diag/source_info.h"
//#include "vox/string/format.h"
//#include "vox/string/string_conv.h"
#include "vox/utils/preprocessor.h"
#include "vox/utils/typed_flag.h"

namespace vox::diag
{
    enum class AssertionKind
    {
        Default,
        Fatal
    };

    enum class FailureAction : unsigned
    {
        None       = 1 << 0,
        DebugBreak = 1 << 1,
        Abort      = 1 << 2
    };

    using FailureActionMask = TypedMask<FailureAction>;
}

namespace vox::diag_detail
{
    template <typename... Args>
    consteval inline std::string_view makeFailureMessage()
    {
        return "";
    }

    template <typename T>
    requires(!std::is_constructible_v<std::string_view, T>) &&
            (!std::is_constructible_v<std::string_view, T>)
    inline auto makeFormatableArgs(T&& arg)
    {
        return std::forward<T>(arg);
    }

    template <typename T>
    requires std::is_constructible_v<std::string_view, T>
    inline const char* makeFormatableArgs(T&& arg)
    {
        const char* res =std::string_view{std::forward<T>(arg)}.data();
        if(!res)
        {
            return "NULLPTR";
        }
        return res;
    }

    template <typename T>
    requires(!std::is_constructible_v<std::string_view, T>) &&
            std::is_constructible_v<std::string_view, T>
    inline const char* makeFormatableArgs(T&& arg)
    {
        const char* res = std::string_view{std::forward<T>(arg)}.data();
        if(!res)
        {
            return "NULLPTR";
        }
        return res;
    }

    template <typename T>
    inline const char* makeFormatableArgs(nullptr_t)
    {
        return "NULLPTR";
    }

    template <typename... Args>
    inline std::string makeFailureMessage(std::u8string_view text, Args&&... formatArgs)
    {
        if constexpr(sizeof...(Args) == 0)
        {
            return reinterpret_cast<const char*>(text.data());
        }
        else
        {
            return std::format(text, makeFormatableArgs<Args>(std::forward<Args>(formatArgs))...);
        }
    }

    template <typename... Args>
    inline std::string makeFailureMessage(std::string_view text, Args&&... formatArgs)
    {
        if constexpr(sizeof...(Args) == 0)
        {
            return std::string{text.data(), text.size()};
        }
        else
        {
            return std::format(text.data(), makeFormatableArgs<Args>(std::forward<Args>(formatArgs))...);
        }
    }

    /**
     */
    vox::diag::FailureAction raiseFailure(uint32_t error, vox::diag::AssertionKind kind,
        vox::diag::SourceInfo source, std::string_view condition, std::string_view message);

}

#define VOX_ASSERT_IMPL(error, kind, condition, ...)                                                                                                                         \
    do                                                                                                                                                                       \
    {                                                                                                                                                                        \
        if(!(condition)) [[unlikely]]                                                                                                                                        \
        {                                                                                                                                                                    \
            const auto errorFlags = ::vox::diag_detail::raiseFailure(error, kind, VOX_INLINED_SOURCE_INFO, #condition, ::vox::diag_detail::makeFailureMessage(__VA_ARGS__)); \
            if(errorFlags.has(::vox::diag::FailureAction::DebugBreak) && ::vox::debug::isRunningUnderDebugger())                                                             \
            {                                                                                                                                                                \
                VOX_PLATFORM_BREAK;                                                                                                                                          \
            }                                                                                                                                                                \
            if(errorFlags.has(::vox::diag::FailureAction::Abort))                                                                                                            \
            {                                                                                                                                                                \
                VOX_PLATFORM_ABORT;                                                                                                                                          \
            }                                                                                                                                                                \
        }                                                                                                                                                                    \
    } while(0)

#define VOX_FAILURE_IMPL(error, kind, ...)                                                                                                                            \
    do                                                                                                                                                                \
    {                                                                                                                                                                 \
        const auto errorFlags = ::vox::diag_detail::raiseFailure(error, kind, VOX_INLINED_SOURCE_INFO, "Failure", ::vox::diag_detail::makeFailureMessage(__VA_ARGS__)); \
        if(errorFlags.has(::vox::diag::FailureAction::DebugBreak) && ::vox::debug::isRunningUnderDebugger())                                                          \
        {                                                                                                                                                             \
            VOX_PLATFORM_BREAK;                                                                                                                                       \
        }                                                                                                                                                             \
        if(errorFlags.has(::vox::diag::FailureAction::Abort))                                                                                                         \
        {                                                                                                                                                             \
            VOX_PLATFORM_ABORT;                                                                                                                                       \
        }                                                                                                                                                             \
    } while(0)
 

#define VOX_ASSERT(condition, ...)
#define VOX_FATAL(condition, ...)
#define VOX_FAILURE(...)
#define VOX_FATAL_FAILURE(...)
#define VOX_FAST_ASSERT(expr)

#define VOX_VERIFY(condition, ...) VOX_ASSERT_IMPL(1, ::vox::diag::AssertionKind::Default, condition, ##__VA_ARGS__)

#define VOX_FAILURE_ALWAYS(...) VOX_FAILURE_IMPL(1, ::vox::diag::AssertionKind::Default, ##__VA_ARGS__)