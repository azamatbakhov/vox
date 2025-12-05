#pragma once

#include <cstdlib>
#include <intrin.h>

#define VOX_PLATFORM_BREAK (__nop(), __debugbreak())
#define VOX_PLATFORM_ABORT (std::abort())
