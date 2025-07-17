#include <auth_configuration/json_configuration.h>
#include <QJsonDocument>
#include <jwt/json/json.hpp>


JsonConfiguration::JsonConfiguration(const QString &configPath) : configPath(configPath) {
    auto file = QFile(this->configPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open config file for read: " << configPath;
        return;
    }

    const auto fileContent = file.readAll();
    this->root = QJsonDocument::fromJson(fileContent).object();
}

void JsonConfiguration::setServiceConfig(const QString &config, const QVariant &value) {
    QJsonObject serviceObj = this->root.value("service").toObject();
    serviceObj[config] = QJsonValue::fromVariant(value);
    this->root["service"] = serviceObj;

    auto file = QFile(this->configPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Configuration \"`service." + config + "\" doesn't saved.";
        return;
    }
    file.write(QJsonDocument(this->root).toJson());
}

QVariant JsonConfiguration::getServiceConfig(const QString &config) const {
    const auto service = this->root["service"];
    if (service.isNull() || !service.isObject()) {
        return {};
    }
    return service[config];
}

void JsonConfiguration::setAuthConfig(const QString &config, const QVariant &value) {
    QJsonObject authObj = this->root.value("auth").toObject();
    authObj[config] = QJsonValue::fromVariant(value);
    this->root["auth"] = authObj;

    auto file = QFile(this->configPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Configuration \"`auth." + config + "\" doesn't saved.";
        return;
    }
    file.write(QJsonDocument(this->root).toJson());
}

QVariant JsonConfiguration::getAuthConfig(const QString &config) const {
    const auto service = this->root["auth"];
    if (service.isNull() || !service.isObject()) {
        return {};
    }
    return service[config];
}

void JsonConfiguration::setUserConfig(const QString &config, const QVariant &value) {
    QJsonObject userObj = this->root.value("user").toObject();
    userObj[config] = QJsonValue::fromVariant(value);
    this->root["user"] = userObj;

    auto file = QFile(this->configPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Configuration \"`user." + config + "\" doesn't saved.";
        return;
    }
    file.write(QJsonDocument(this->root).toJson());
}

QVariant JsonConfiguration::getUserConfig(const QString &config) const {
    const auto userObj = this->root["user"];
    if (userObj.isNull() || !userObj.isObject()) {
        return {};
    }
    return userObj[config];
}

JsonConfiguration loadConfiguration() {
    static const char *env = std::getenv("JRPC_AUTH_CONFIG_PATH");
    static const QString configuration_path = env ? env : "~/.config/jrpc_auth/config.json";
    return JsonConfiguration(configuration_path);
}
