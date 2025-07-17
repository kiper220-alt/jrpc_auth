#ifndef AUTH_CONFIGURATION_H
#define AUTH_CONFIGURATION_H

#include <QFile>
#include <QJsonObject>
#include <auth_configuration/iuser_config.h>
#include <auth_configuration/iauth_config.h>
#include <auth_configuration/iservice_config.h>

/**
 * @brief This class provides access to configuration file in JSON format.
 * - Configuration MAY doesn't exist,
 * - Configuration MAY contain empty object ("{}"),
 * - Configuration CANNOT be invalid JSON,
 * - Configuration MAY contain "service", "auth" and "user" objects,
 * - Configuration CANNOT contain "service", "auth" and "user" elements.
 * - Configuration MAY contain "service.<name>", "auth.<name>" and "user.<name>" elements,
 */
class JsonConfiguration : public IUserConfig, public IAuthConfig, public IServiceConfig {
public:
    /**
     * @brief JsonConfiguration
     * @param configPath path to config file
     */
    explicit JsonConfiguration(const QString &configPath = "~/.config/jrpc_auth/config.json");

    /**
     * @brief set service related configurations.
     * @param config configuration name to set
     * @param value configuration value to set
     */
    void setServiceConfig(const QString &config, const QVariant &value) override;

    /**
     * @brief get service related configurations. This configuration is placed in "service" object from root of config
     * file
     * @param name configuration name
     * @return "service.<name>" configuration.
     */
    [[nodiscard]] QVariant getServiceConfig(const QString &config) const override;

    /**
     * @brief set auth related configurations.
     * @param config configuration name to set
     * @param value configuration value to set
     */
    void setAuthConfig(const QString &config, const QVariant &value) override;

    /**
     * @brief get auth related configurations. This configuration is placed in "auth" object from root of config file
     * @param config configuration name
     * @return "auth.<config>" configuration
     */
    [[nodiscard]] QVariant getAuthConfig(const QString &config) const override;

    /**
     * @brief set user related configurations.
     * @param config configuration name to set
     * @param value configuration value to set
     */
    void setUserConfig(const QString &config, const QVariant &value) override;

    /**
     * @brief get user related configurations. This configuration is placed in "user" object from root of config file
     * @param config configuration name
     * @return "user.<config>" configuration
     */
    [[nodiscard]] QVariant getUserConfig(const QString &config) const override;

    virtual ~JsonConfiguration() = default;

private:
    QString configPath;
    QJsonObject root;
};

/**
 * @brief loading configuration from default path.
 * Default path is "~/.config/jrpc_auth/config.json".
 * Default path can be overridden by environment variable "JRPC_AUTH_CONFIG_PATH".
 * @return configuration from default path
 */
JsonConfiguration loadConfiguration();

#endif // AUTH_CONFIGURATION_H
