#include <auth_service.h>
#include <jwt/jwt.hpp>

std::string getSomeSecret() {
    return "SomeSecret";
}

explicit AuthService::AuthService(AuthServiceSettings &&settings, QObject *parent = nullptr) : QJsonRpcService(parent),
                                                                                               auths(std::move(settings.authStorage)),
                                                                                               users(std::move(settings.userStorages))
{
}

QVariantMap AuthService::login(const QString &username, const QString &password) {
    for (const auto &userStorage: this->users) {
        if (const auto authData = userStorage->authenticate(username, password); authData.has_value()) {
            auto auth_id = this->auths->authenticate(username, authData.value());
            jwt::jwt_object token{jwt::params::algorithm("HS256"), jwt::params::secret(getSomeSecret())};
            token.add_claim("exp", std::chrono::system_clock::now() + std::chrono::minutes(60));
            token.add_claim("iss", username);
            token.add_claim("jti", auth_id);

            return QVariantMap({{"token", QVariant::fromValue(token.signature())}, {"user", QVariant::fromValue(authData.value())}});
        }
    }
    return QVariantMap({"error", "invalid username or password"});
}

void AuthService::logout(const QString &token) {
    auto std_string = token.toStdString();
    // jwt::decode(std::string_view(std_string), jwt::params::algorithm("HS256"), jwt::params::secret(getSomeSecret()));
}

bool checkAuth(const QString &token);

QVariantMap getIdentity(const QString &token);
