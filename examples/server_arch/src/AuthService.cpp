
#include "AuthService.h"

AuthService::AuthService(std::unique_ptr<IAuthStorage> storage, QObject *parent) : QJsonRpcService(parent),
    storage(std::move(storage)) {
}

/// TODO: This code has userinfo merge conflict.
QVariantMap AuthService::login(const QString &username, const QString &password) {
    QString id;
    QVariantMap userInfo;
    for (const auto &userStorage: this->authStorages) {
        if (const auto authData = userStorage->authenticate(username, password); authData.has_value()) {
            if (!id.isEmpty()) {
                id += ';';
            } else {
                userInfo = authData.value().user;
            }
            id += userStorage->getName() + "~=" + authData.value().id;
        }
    }
    if (id.isEmpty()) {
        return QVariantMap({"error", "invalid username or password"});
    }
    return QVariantMap({{"token", this->storage->authenticate(username)}, {"user", userInfo}});
}

void AuthService::logout(const QString &token) {
    this->storage->removeToken(token);
}

/// TODO: no verify in UserStorage
bool AuthService::checkAuth(const QString &token) {
    if (this->storage->getUserName(token)) {
        return true;
    }
    return false;
}

/// TODO: No token convert to userid and next userinfo merge conflict resolve.
QVariantMap AuthService::getIdentity(const QString &token) {
    return QVariantMap({});
}


void addUserStorage(AuthService &authService, std::unique_ptr<IUserStorage> userStorage) {
    authService.authStorages.emplace_back(std::move(userStorage));
}
