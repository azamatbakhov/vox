#pragma once

#include <boost/uuid/uuid.hpp>

boost::uuids::uuid generateUuid();

boost::uuids::uuid uuidFromBytes(std::string const& bytes);
boost::uuids::uuid uuidFromString(std::string const& str);

std::string uuidToBytes(boost::uuids::uuid const& uuid);
std::string uuidToString(boost::uuids::uuid const& uuid);
