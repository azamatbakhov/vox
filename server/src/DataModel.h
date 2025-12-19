#pragma once

#include "vox/rtti/rtti_impl.h"
#include "vox/rtti/rtti_object.h"


#include <boost/uuid.hpp>
#include <pqxx/pqxx>
#include <optional>


namespace vox::db
{
    struct User
    {
        std::size_t id{};
        std::string login;
        boost::uuids::uuid uuid;
        std::string firstName;
        //...
    };

    struct Chats
    {
      std::size_t id{};
      //...
    };

}


struct
    IDataModel : vox::IRttiObject
{
    VOX_TYPEID(IDataModel)

    ~IDataModel(){}


    virtual std::optional<::vox::db::User> getUser(const std::string& login) = 0;
    virtual std::vector<::vox::db::Chats> getUserChats(const boost::uuids::uuid& userUuid) = 0;
};


class DataModel : public IDataModel
{
    VOX_CLASS(DataModel, IDataModel)

    explicit DataModel(const std::string& connectStr);

    virtual std::optional<::vox::db::User> getUser(const std::string& login) override;
    virtual std::vector<::vox::db::Chats> getUserChats(const boost::uuids::uuid& userUuid) override;

private:
    void printStatistics();

    pqxx::connection m_pgConnection;
    pqxx::work m_pgWork;
};


