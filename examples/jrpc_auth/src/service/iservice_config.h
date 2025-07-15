#ifndef ISERVICE_CONFIG_H
#define ISERVICE_CONFIG_H

#include <QJsonValue>

class IServiceConfig {
public:
    /**
     * @brief set service related configurations.
     * @param config configuration name to set
     * @param value configuration value to set
     */
    virtual void setServiceConfig(const QString &config, const QVariant &value) = 0;

    /**
     * @brief get service related configurations.
     * @param config configuration name to get
     * @return "service.<name>" configuration.
     */
    [[nodiscard]] virtual QVariant getServiceConfig(const QString &config) const = 0;

    virtual ~IServiceConfig() = default;
};

#endif // ISERVICE_CONFIG_H
