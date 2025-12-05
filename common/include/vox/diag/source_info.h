#pragma once

#include <optional>
#include <string_view>

#include "vox/utils/preprocessor.h"

namespace vox::diag
{
    struct SourceInfo
    {
        std::string_view moduleName;
        std::string_view functionName;
        std::string_view filePath;
        std::optional<unsigned> line;

        SourceInfo() = default;
        SourceInfo(const SourceInfo&) = default;

        SourceInfo(std::string_view module, std::string_view func, std::string_view file,
            std::optional<unsigned> ln = std::nullopt) :
            moduleName(module),
            functionName(func),
            filePath(file),
            line(ln)
        {
        }

        SourceInfo(std::string_view func, std::string_view file, std::optional<unsigned> ln = std::nullopt) :
            SourceInfo{{}, func, file, ln}
        {
        }

        SourceInfo& operator=(const SourceInfo&) = default;

        explicit operator bool() const
        {
            return !functionName.empty() || !filePath.empty();
        }
    };

}

#define VOX_INLINED_SOURCE_INFO                                                                             \
    ::vox::diag::SourceInfo                                                                                 \
    {                                                                                                       \
        std::string_view{__FUNCTION__}, std::string_view{__FILE__}, static_cast<unsigned>(__LINE__) \
    }
