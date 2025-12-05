#pragma once

#include <string_view>

namespace vox
{

    constexpr std::string_view pkgDelimiter()
    {
        return "@@@";
    }

    constexpr std::size_t pkgDelimiterSize()
    {
        return pkgDelimiter().size();
    }

}  // namespace vox