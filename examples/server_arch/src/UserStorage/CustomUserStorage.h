
#ifndef CUSTOMUSERSTORAGE_H
#define CUSTOMUSERSTORAGE_H
#include <common.h>

struct User {
    QString username;
    QString password;
    QString email;
};

class CustomUserStorage : public IUserStorage {
    const QString &getName() override;

    std::optional<AuthData> authenticate(const QString &username, const QString &password) override;

    std::optional<QVariantMap> getUserInfo(const QString &id) override;

    ~CustomUserStorage() override = default;
};


#endif //CUSTOMUSERSTORAGE_H
