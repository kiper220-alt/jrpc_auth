#include "auth_service.h"
#include <qjsonrpc/qjsonrpcservice.h>
#include <jwt/jwt.hpp>

static QString createJwtToken(const QString &secret, const QString &jti, const QString &issuer, const QString &username) {
    jwt::jwt_object data{jwt::params::algorithm("HS256"), jwt::params::secret(secret.toStdString())};
    auto current_time = std::chrono::system_clock::now();

    data.add_claim(jwt::registered_claims::jti, jti.toStdString());
    data.add_claim(jwt::registered_claims::issuer, issuer.toStdString());
    data.add_claim(jwt::registered_claims::subject, username.toStdString());
    data.add_claim(jwt::registered_claims::issued_at, current_time);

    data.secret(secret.toStdString());
    return QString::fromStdString(data.signature());
}

static std::optional<QString> verifyJwtAndGetToken(const QString &jwt,
                                                   const QString &secret) noexcept {
    std::error_code ec;
    jwt::jwt_object data = jwt::decode(
            jwt.toStdString(),
            jwt::params::algorithms({"HS256"}),
            ec, jwt::params::secret(secret.toStdString()), jwt::params::verify(true));

    if (ec) {
        return std::nullopt;
    }
    return QString::fromStdString(data.payload().get_claim_value<std::string>(jwt::registered_claims::jti));
}

AuthService::AuthService(
        AuthServiceSettings &&settings,
        IServiceConfig *config,
        QObject *parent
) : QJsonRpcService(parent),
    auths(std::move(settings.authStorage)),
    users(std::move(settings.userStorages)),
    name(config ? config->getServiceConfig("name").toString() : "auth"),
    secret(config ? config->getServiceConfig("secret").toString() : "SOME_JWT_SECRET") {
}

QJsonObject AuthService::login(const QString &username, const QString &password) {
    auto request = currentRequest();
    for (auto &user: users) {
        auto auth = user->authenticate(username, password);
        if (auth.has_value()) {
            QString token = this->auths->authenticate(username, auth.value());
            QString jwtToken = createJwtToken(this->secret, token, this->name, username);

            if (token.isEmpty()) {
                const auto error = request.request().createErrorResponse(
                    QJsonRpc::InternalError, "Internal server error");
                emit result(error);
                return {};
            }

            return QJsonObject::fromVariantMap({
                {"token", jwtToken},
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
    auto jti = verifyJwtAndGetToken(token, this->secret);
    if (!jti) {
        return false;
    }
    return this->auths->remove(jti.value());
}

bool AuthService::checkAuth(const QString &token) {
    auto jti = verifyJwtAndGetToken(token, this->secret);
    if (!jti) {
        return false;
    }
    auto user = this->auths->get(jti.value());
    if (!user) {
        return false;
    }
    for (auto &ustorage: this->users) {
        if (ustorage->getUserVersion(user->first) == user->second) {
            return true;
        }
    }
    this->auths->remove(jti.value());
    return false;
}

QJsonObject AuthService::getIdentity(const QString &token) {
    auto request = currentRequest();
    auto jti = verifyJwtAndGetToken(token, this->secret);
    if (!jti) {
        auto error = request.request().createErrorResponse(QJsonRpc::InvalidParams, "Invalid token");
        emit result(error);
        return {};
    }
    auto user = auths->get(jti.value());
    if (!user) {
        auto error = request.request().createErrorResponse(QJsonRpc::InvalidParams, "Is out of use");
        emit result(error);
        return {};
    }
    return {{"username", user->first}};
}
