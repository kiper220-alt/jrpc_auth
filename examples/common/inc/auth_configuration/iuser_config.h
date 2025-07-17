#ifndef IUSERCONFIG_H
#define IUSERCONFIG_H

#include <QJsonValue>

class IUserConfig {
public:
    /**
     * @brief set user related configurations.
     * @param config configuration name to set
     * @param value configuration value to set
     */
    virtual void setUserConfig(const QString &config, const QVariant &value) = 0;

    /**
     * @brief get user related configurations.
     * @param config configuration name to get
     * @return "user.<name>" configuration.
     */
    [[nodiscard]] virtual QVariant getUserConfig(const QString &config) const = 0;

    virtual ~IUserConfig() = default;
};

#endif // IUSERCONFIG_H
