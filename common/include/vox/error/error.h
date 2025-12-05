#pragma once

#include "vox/rtti/rtti_impl.h"
#include <boost/system/error_code.hpp>

namespace vox
{
    
enum class ErrorCode
{
    None,
    NotAllBytesSent,
};

class Error final : virtual vox::IRttiObject
{
    VOX_CLASS(Error)

    Error() = default;
    Error(ErrorCode code);
    Error(std::exception const& ec);

    ErrorCode code() const { return m_code; }
    std::string_view description() const { return m_description; }
    
    static Error fromBoostErrorCode(const boost::system::error_code& error);
    
    explicit operator bool() const
    {
        return m_code != ErrorCode::None;
    }

private:
    ErrorCode m_code{};
    std::string m_description;
};

}