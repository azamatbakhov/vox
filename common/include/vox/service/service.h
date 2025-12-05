#pragma once

#include "vox/rtti/rtti_object.h"

namespace vox
{
    struct VOX_ABSTRACT_TYPE IServiceInitialization : virtual IRttiObject
    {
        VOX_INTERFACE(vox::IServiceInitialization, IRttiObject)
    };
}
