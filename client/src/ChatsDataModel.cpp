#include "ChatsDataModel.h"

ChatsDataModel::ChatsDataModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

void ChatsDataModel::setChats(std::vector<Chat> chats)
{
    beginResetModel();
    m_chats = std::move(chats);
    endInsertRows();
}

int ChatsDataModel::rowCount(const QModelIndex&) const
 {
  return m_chats.size();
}

QVariant ChatsDataModel::data(const QModelIndex & index, int role) const
 {
    if (index.row() < 0 || index.row() >= m_chats.size())
    {
        return QVariant();
    }

    const Chat& chat = m_chats[index.row()];

    // ...

    return QVariant();
}

QHash<int, QByteArray> ChatsDataModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[TypeRole] = "type";
    return roles;
}
