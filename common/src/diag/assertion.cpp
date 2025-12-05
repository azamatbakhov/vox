#include "vox/diag/assertion.h"

#include "vox/debug/debugger.h"
#include "vox/utils/scope_guard.h"
#include "vox/utils/enum.h"

namespace vox::diag_detail
{
    diag::FailureAction raiseFailure(uint32_t error, diag::AssertionKind kind, 
        diag::SourceInfo source, std::string_view condition, std::string_view message)
    {
        using namespace vox::diag;

        static thread_local unsigned threadRaiseFailureCounter = 0;

        if(threadRaiseFailureCounter > 0)
        {
            VOX_PLATFORM_BREAK;
            VOX_PLATFORM_ABORT;
        }

        ++threadRaiseFailureCounter;

        scope_on_leave
        {
            --threadRaiseFailureCounter;
        };

        return kind == AssertionKind::Default
            ? FailureAction::DebugBreak 
            : (FailureAction::DebugBreak | FailureAction::Abort);
    }
}