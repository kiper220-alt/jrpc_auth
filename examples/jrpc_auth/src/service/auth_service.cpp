#include "auth_service.h"
#include <qjsonrpc/qjsonrpcservice.h>
#include <jwt/jwt.hpp>

AuthService::AuthService(AuthServiceSettings &&settings, QObject *parent) : QJsonRpcService(parent),
                                                                            auths(std::move(settings.authStorage)),
                                                                            users(std::move(settings.userStorages)) {
}

QJsonObject AuthService::login(const QString &username, const QString &password) {
    auto request = currentRequest();
    for (auto &user: users) {
        auto auth = user->authenticate(username, password);
        if (auth.has_value()) {
            QString token = this->auths->authenticate(username, auth.value());
            if (token.isEmpty()) {
                const auto error = request.request().createErrorResponse(
                    QJsonRpc::InternalError, "Internal server error");
                emit result(error);
                return {};
            }
            return QJsonObject::fromVariantMap({
                {"token", token},
                {
                    "user",
                    QVariant::fromValue(QVariantMap({
                        {"username", username},
                    }))
                }
            });
        }
    }
    const auto error = request.request().createErrorResponse(QJsonRpc::InternalError, "Invalid username or password");
    emit result(error);
    return {};
}

bool AuthService::logout(const QString &token) {
    return this->auths->remove(token);
}

bool AuthService::checkAuth(const QString &token) {
    auto user = this->auths->get(token);
    if (!user) {
        return false;
    }
    for (auto &ustorage: this->users) {
        if (ustorage->getUserVersion(user->first) == user->second) {
            return true;
        }
    }
    this->auths->remove(token);
    return false;
}

QJsonObject AuthService::getIdentity(const QString &token) {
    auto request = currentRequest();
    auto user = auths->get(token);
    if (!user) {
        auto error = request.request().createErrorResponse(QJsonRpc::InvalidParams, "Invalid token");
        emit result(error);
        return {};
    }
    return {{"username", user->first}};
}
