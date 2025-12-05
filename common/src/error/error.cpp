#include "vox/error/error.h"

namespace vox
{

Error::Error(ErrorCode code)
    : m_code(code)
{

}

Error::Error(std::exception const& ec) :
    m_description(ec.what())
{

}
    
Error Error::fromBoostErrorCode(const boost::system::error_code& error)
{
    return {};
}

}