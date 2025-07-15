#ifndef IAUTHCONFIG_H
#define IAUTHCONFIG_H

#include <QJsonValue>

class IAuthConfig {
public:
    /**
     * @brief set auth related configurations.
     * @param config configuration name to set
     * @param value configuration value to set
     */
    virtual void setAuthConfig(const QString &config, const QVariant &value) = 0;

    /**
     * @brief get auth related configurations.
     * @param config configuration name to get
     * @return "auth.<name>" configuration.
     */
    [[nodiscard]] virtual QVariant getAuthConfig(const QString &config) const = 0;

    virtual ~IAuthConfig() = default;
};

#endif // IAUTHCONFIG_H
