#include "vox/utils/uuid.h"

#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <cstring>
#include <stdexcept>

boost::uuids::uuid generateUuid()
{
    thread_local boost::uuids::random_generator gen;
    return gen();
}

boost::uuids::uuid uuidFromBytes(std::string const& bytes)
{
    if (bytes.size() != boost::uuids::uuid::static_size())
    {
        return boost::uuids::nil_uuid();
        //throw std::length_error("Trying to initialize a UUID with the wrong number of bytes");
    }

    boost::uuids::uuid uuid;
    memcpy(uuid.data, bytes.data(), boost::uuids::uuid::static_size());

    return uuid;
}

boost::uuids::uuid uuidFromString(std::string const& str)
{
    boost::uuids::uuid result = boost::uuids::nil_uuid();

    try
    {
        result = boost::uuids::string_generator()(str.begin(), str.end());
    }
    catch (std::exception const&)
    {
        // Boost just throws std::runtime_error("invalid uuid string"),
        // without specifiying which string was invalud.
        std::ostringstream strm;
        strm << "Invalid UUID: '" << str << '\'';
    }

    return result;
}

std::string uuidToBytes(boost::uuids::uuid const& uuid)
{
    return std::string(uuid.begin(), uuid.end());
}

std::string uuidToString(boost::uuids::uuid const& uuid)
{
    return boost::uuids::to_string(uuid);
}
