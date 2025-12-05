#pragma once

#include "vox/utils/preprocessor.h"
#include VOX_PLATFORM_HEADER(platform_debug.h)

namespace vox::debug
{
    bool isRunningUnderDebugger();

    inline void debugBreak()
    {
        VOX_PLATFORM_BREAK;
    }
}
