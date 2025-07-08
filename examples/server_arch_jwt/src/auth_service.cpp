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

}

void AuthService::logout(const QString &token) {
}

bool checkAuth(const QString &token);

QVariantMap getIdentity(const QString &token);
