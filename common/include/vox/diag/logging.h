#pragma once
 
#include <type_traits>
#include <iostream>

namespace vox::diag
{
    enum class LogLevel : uint16_t
    {
        /** Prints a info in Debug configuration to console (and log file) */
        Debug,

        /** Prints a info to console (and log file) */
        Info,

        /** Prints a warning to console (and log file) */
        Warning,

        /** Sends an error and crashes in release builds */
        Error,

        /** Sends a fatal error and crashes */
        Critical,

        /** Sends a verbose message (if Verbose logging is enabled), usually used for detailed logging. */
        Verbose,
    };
}


#define VOX_LOG_MESSAGE(criticality, formatStr, ...)                     \
    {                                                                    \
        std::cout << std::format(formatStr, ##__VA_ARGS__) << std::endl; \
    }                                                                    \

#define VOX_LOG_INFO(str, ...) VOX_LOG_MESSAGE(::vox::diag::LogLevel::Info, str, ##__VA_ARGS__)
#define VOX_LOG_DEBUG(str, ...) VOX_LOG_MESSAGE(::vox::diag::LogLevel::Info, str, ##__VA_ARGS__)
#define VOX_LOG_WARNING(str, ...) VOX_LOG_MESSAGE(::vox::diag::LogLevel::Info, str, ##__VA_ARGS__)
#define VOX_LOG_ERROR(str, ...) VOX_LOG_MESSAGE(::vox::diag::LogLevel::Info, str, ##__VA_ARGS__)
#define VOX_LOG_CRITICAL(str, ...) VOX_LOG_MESSAGE(::vox::diag::LogLevel::Info, str, ##__VA_ARGS__)


#define VOX_CONDITION_LOG(condition, criticality, formatStr, ...) \
    {                                                                   \
        if (!!(condition))                                              \
        {                                                               \
            VOX_LOG_MESSAGE(criticality, formatStr, ##__VA_ARGS__)      \
        }                                                               \
    }
