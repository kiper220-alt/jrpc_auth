#include <auth_service.h>
#include <QFile>
#include <qjsonrpc/qjsonrpcservice.h>
#include <jwt/jwt.hpp>

static QString createJwtToken(const QString &secret, const QString &jti, const QString &issuer,
                              const QString &username) {
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

static QString createTokenImpl(
    const QString &secret, const QString &jti, const QString &issuer,
    const QString &subject, const QString &audience, bool refresh,
    const std::chrono::system_clock::time_point &issued_at,
    const std::chrono::system_clock::time_point &not_before,
    const std::chrono::system_clock::time_point &expiration
) {
    jwt::jwt_object jwt_object{
        jwt::params::algorithm("RS256"),
        jwt::params::secret(secret.toStdString()),
    };

    jwt_object.add_claim(jwt::registered_claims::audience, audience.toStdString());
    jwt_object.add_claim(jwt::registered_claims::expiration, expiration);
    jwt_object.add_claim(jwt::registered_claims::issued_at, issued_at);
    jwt_object.add_claim(jwt::registered_claims::issuer, issuer.toStdString());
    jwt_object.add_claim(jwt::registered_claims::jti, jti.toStdString());
    jwt_object.add_claim(jwt::registered_claims::not_before, not_before);
    jwt_object.add_claim(jwt::registered_claims::subject, subject.toStdString());
    jwt_object.add_claim("ref", refresh);

    return QString::fromStdString(jwt_object.signature());
}

QPair<QString, QString> AuthService::createTokens(const QString &username, const QString &audience) const {
    QPair<QString, QString> pair;
    const QString token = this->auths->authenticate(username, audience);

    // refresh
    pair.first = createTokenImpl(this->privateKey, token, this->serviceName,
                                 username, audience, true,
                                 std::chrono::system_clock::now(),
                                 std::chrono::system_clock::now() + std::chrono::minutes(10),
                                 std::chrono::system_clock::now() + std::chrono::hours(24));
    // access
    pair.second = createTokenImpl(this->privateKey, token, this->serviceName,
                                  username, audience, false,
                                  std::chrono::system_clock::now(),
                                  std::chrono::system_clock::now(),
                                  std::chrono::system_clock::now() + std::chrono::minutes(5));

    return pair;
}

std::optional<QPair<QString, QString> > AuthService::newPairFromRefresh(const QString &refreshToken) const {
    // parse refresh token
    std::error_code ec;
    jwt::jwt_object data = jwt::decode(
        refreshToken.toStdString(),
        jwt::params::algorithms({"RS256"}),
        ec,
        jwt::params::secret(this->publicKey.toStdString()));

    // if refresh token is invalid, return
    if (ec || !data.payload().get_claim_value<bool>("ref")) {
        return std::nullopt;
    }

    // get all claims from refresh token
    const auto jti = QString::fromStdString(data.payload().get_claim_value<std::string>(jwt::registered_claims::jti));
    const auto audience = QString::fromStdString(
        data.payload().get_claim_value<std::string>(jwt::registered_claims::audience));
    const auto username = QString::fromStdString(
        data.payload().get_claim_value<std::string>(jwt::registered_claims::subject));

    // if jti is not exists, return
    const auto user = this->auths->get(jti);
    if (!user) {
        return std::nullopt;
    }

    // remove jti anyway
    this->auths->remove(jti);

    // if user version is changed, return
    auto found = false;
    for (const auto &ustorage: this->users) {
        if (ustorage->getUserVersion(user->first) != user->second) {
            found = true;
            break;
        }
    }
    if (!found) {
        return std::nullopt;
    }

    // create new tokens
    return this->createTokens(username, audience);
}

AuthService::AuthService(
    AuthServiceSettings &&settings,
    const IServiceConfig *config,
    QObject *parent
) : QJsonRpcService(parent),
    users(std::move(settings.userStorages)),
    auths(std::move(settings.authStorage)),
    serviceName(config ? config->getServiceConfig("name").toString() : "auth") {
    const auto privateKeyPath = config->getServiceConfig("private_key").toString();
    const auto publicKeyPath = config->getServiceConfig("public_key").toString();

    // private key
    {
        auto file = QFile(privateKeyPath);
        if (!file.open(QIODevice::ReadOnly)) {
            qFatal("Failed to open private key file for read: `%s`", privateKeyPath.toStdString().c_str());
        }
        this->privateKey = file.readAll();
        file.close();
    }
    // public key
    {
        auto file = QFile(publicKeyPath);
        if (!file.open(QIODevice::ReadOnly)) {
            qFatal("Failed to open public key file for read: `%s`", publicKeyPath.toStdString().c_str());
        }
        this->publicKey = file.readAll();
        file.close();
    }
}

QJsonObject AuthService::login(const QString &username, const QString &password, const QString &audience) {
    const auto request = currentRequest();

    for (const auto &user: users) {
        if (const auto auth = user->authenticate(username, password); auth.has_value()) {
            const QString token = this->auths->authenticate(username, auth.value());
            QPair<QString, QString> pair = this->createTokens(username, audience);

            if (token.isEmpty()) {
                const auto error = request.request().createErrorResponse(
                    QJsonRpc::InternalError, "Internal server error");
                emit result(error);
                return {};
            }

            return QJsonObject::fromVariantMap({
                {"refresh", pair.first},
                {"access", pair.second},
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

QJsonObject AuthService::refresh(const QString &token) {
    const auto request = currentRequest();
    const auto pair = this->newPairFromRefresh(token);
    if (!pair) {
        const auto error = request.request().createErrorResponse(
            QJsonRpc::InvalidParams, "Invalid refresh token");
        emit result(error);
        return {};
    }

    return QJsonObject::fromVariantMap({
        {"refresh", pair->first},
        {"access", pair->second},
    });
}

bool AuthService::logout(const QString &token) {
    const auto request = currentRequest();
    std::error_code ec;
    jwt::jwt_object data = jwt::decode(
        token.toStdString(),
        jwt::params::algorithms({"RS256"}),
        ec,
        jwt::params::secret(this->publicKey.toStdString()));

    if (ec) {
        const auto error = request.request().createErrorResponse(
            QJsonRpc::InvalidParams, "Invalid token");
        emit result(error);
        return {};
    }

    QString jti = QString::fromStdString(data.payload().get_claim_value<std::string>(jwt::registered_claims::jti));
    return this->auths->remove(jti);
}

bool AuthService::checkAuth(const QString &token) {
    const auto request = currentRequest();
    std::error_code ec;
    jwt::jwt_object data = jwt::decode(
        token.toStdString(),
        jwt::params::algorithms({"RS256"}),
        ec,
        jwt::params::secret(this->publicKey.toStdString()));

    if (ec) {
        const auto error = request.request().createErrorResponse(
            QJsonRpc::InvalidParams, "Invalid token");
        emit result(error);
        return {};
    }

    QString jti = QString::fromStdString(data.payload().get_claim_value<std::string>(jwt::registered_claims::jti));
    return this->auths->get(jti).has_value();
}

QJsonObject AuthService::getIdentity(const QString &token) {
    const auto request = currentRequest();
    std::error_code ec;
    jwt::jwt_object data = jwt::decode(
        token.toStdString(),
        jwt::params::algorithms({"RS256"}),
        ec,
        jwt::params::secret(this->publicKey.toStdString()));

    if (ec) {
        const auto error = request.request().createErrorResponse(
            QJsonRpc::InvalidParams, "Invalid token");
        emit result(error);
        return {};
    }

    const auto jti = QString::fromStdString(data.payload().get_claim_value<std::string>(jwt::registered_claims::jti));
    auto user = this->auths->get(jti);
    if (!user) {
        const auto error = request.request().createErrorResponse(
            QJsonRpc::InternalError, "Internal server error");
        emit result(error);
        return {};
    }
    return {{"username", user->first}};
}
