
#include <UserStorage/CustomUserStorage.h>

constexpr QHash<QString, User> users = {
    {"id1", {"user1", "pass1", "email1"}},
    {"id2", {"user2", "pass2", "email2"}},
    {"id3", {"user3", "pass3", "email3"}},
    {"id4", {"user4", "pass4", "email4"}}
};
constexpr QHash<QString, QString> username2id = {
    {"user1", "id1"},
    {"user2", "id2"},
    {"user3", "id3"},
    {"user4", "id4"}
};


const QString &CustomUserStorage::getName() override {
    static const QString name = "CustomUserStorage";
    return name;
}

std::optional<AuthData> CustomUserStorage::authenticate(const QString &username, const QString &password) override {
    AuthData result;
    if (const auto it = username2id.find(username); it != username2id.end()) {
        const auto &user = users.find(it.value());
        if (user != users.end() && user->password == password) {
            result.id = it.value();
            result.user = QVariantMap({{"email", user->email}, {"name", username}});
            return result;
        }
    }
    return std::nullopt;
}

std::optional<QVariantMap> CustomUserStorage::getUserInfo(const QString &id) override {
    if (const auto it = users.find(id); it != users.end()) {
        return QVariantMap({{"email", it->email}, {"name", it->username}});
    }
    return std::nullopt;
}
