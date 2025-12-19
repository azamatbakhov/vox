#pragma once

#include "Chat.h"
#include <QAbstractListModel>
#include <QStringList>

class ChatsDataModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum ChatsRoles {
        TypeRole = Qt::UserRole + 1,
    };

    ChatsDataModel(QObject* parent = 0);

    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    void setChats(std::vector<Chat> chats);

protected:
    virtual QHash<int, QByteArray> roleNames() const override;
    
private:
    std::vector<Chat> m_chats;
};


