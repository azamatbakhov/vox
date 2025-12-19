#include "DataModel.h"
#include "vox/diag/logging.h"
#include "vox/utils/uuid.h"

DataModel::DataModel(const std::string& connectStr)
try 
    : m_pgConnection(connectStr)
    , m_pgWork(m_pgConnection)
{
    VOX_LOG_DEBUG("Successfully connected to {}", connectStr);

    printStatistics();
}
catch (const std::exception& ex)
{
    VOX_LOG_ERROR("Failed to connect to DataModel {} {}", connectStr, ex.what());
}


std::optional<::vox::db::User> DataModel::getUser(const std::string& login)
{
    try
    {
        const pqxx::result result = m_pgWork
            .exec("SELECT id, uuid, first_name from users where login=$1", pqxx::params{login});

        m_pgWork.commit();

        if (result.empty())
        {
            return std::nullopt;
        }
        
        // CHECK if only one user in db.

        return ::vox::db::User
        {
            .id = result[0][0].as<std::size_t>(),
            .login = login,
            .uuid = uuidFromString(result[0][1].as<std::string>()),
            .firstName = result[0][2].c_str(),
        };
    }
    catch (const std::exception& ex)
    {
        VOX_LOG_ERROR("Failed to gather a statistics {}", ex.what());
    }

    return std::optional<::vox::db::User>();
}

std::vector<::vox::db::Chats> DataModel::getUserChats(const boost::uuids::uuid &userUuid)
{
    try
    {
        const pqxx::result result = m_pgWork
            .exec("SELECT id, first_name from users where login=$1", pqxx::params{login});

        m_pgWork.commit();

        if (result.empty())
        {
            return std::nullopt;
        }
        
        // CHECK if only one user in db.

        return ::vox::db::User
        {
            .id = result[0][0].as<std::size_t>(),
            .login = login,
            .uuid = uuidFromString(result[0][1].as<std::string>()),
            .firstName = result[0][2].c_str(),
        };
    }
    catch (const std::exception& ex)
    {
        VOX_LOG_ERROR("Failed to gather a statistics {}", ex.what());
    }

    return std::optional<::vox::db::User>();

  return std::vector<::vox::db::Chats>();
}


void DataModel::printStatistics()
{
    try
    {
        const pqxx::row r = m_pgWork.exec1("SELECT count(*) from test");
        m_pgWork.commit();
        
        VOX_LOG_INFO("Record count in test table = {}", r[0].as<int>());
    }
    catch (const std::exception& ex)
    {
        VOX_LOG_ERROR("Failed to gather a statistics {}", ex.what());
    }
}