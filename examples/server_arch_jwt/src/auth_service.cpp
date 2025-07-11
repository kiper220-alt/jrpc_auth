#include <auth_service.h>
#include <jwt/jwt.hpp>

AuthService::AuthService(AuthServiceSettings &&settings, QObject *parent) : QJsonRpcService(parent),
                                                                                               auths(std::move(settings.authStorage)),
                                                                                               users(std::move(settings.userStorages))
{
}

QVariantMap AuthService::login(const QString &username, const QString &password) {
    for (auto &user: users) {
        auto auth = user->authenticate(username, password);
        if (auth.has_value()) {
            QString token = this->auths->authenticate(username, auth.value());
            if (token.isEmpty()) {
                return {{"error", "Internal server error"}};
            }
            return {
                {"token", token},
                {
                    "user",
                    QVariant::fromValue(QVariantMap({
                        {"username", username},
                    }))
                }
            };
        }
    }
    return {{"error", "Invalid username or password"}};
}

bool AuthService::logout(const QString &token) {
    return this->auths->remove(token);
}

bool AuthService::checkAuth(const QString &token) {
    auto user = auths->get(token);
    if (!user) {
        return false;
    }
    for (auto &ustorage : users) {
        if (ustorage->getUserVersion(user->first) == user->second) {
            return true;
        }
    }
    return false;
}

QVariantMap AuthService::getIdentity(const QString &token) {
    auto user = auths->get(token);
    if (!user) {
        return {{"error", "Invalid token"}};
    }
    return {{"username", user->first}, {"version", user->second}};
}
