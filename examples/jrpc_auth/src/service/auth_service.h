#ifndef AUTH_SERVICE_H
#define AUTH_SERVICE_H

#include <qjsonrpc/qjsonrpcservice.h>
#include "auth_storage/iauth_storage.h"
#include "user_storage/iuser_storage.h"
#include "iservice_config.h"

typedef struct AuthServiceSettings {
    std::unique_ptr<IAuthStorage> authStorage;
    std::vector<std::unique_ptr<IUserStorage> > userStorages;

    ~AuthServiceSettings() = default;
} AuthServiceSettings;

class AuthService : public QJsonRpcService {
    Q_OBJECT
    Q_CLASSINFO("serviceName", "auth")

public:
    AuthService(const AuthService &) = delete;

    /// @brief Constructor
    /// @param settings authentication settings
    explicit AuthService(AuthServiceSettings &&settings, IServiceConfig *config = nullptr, QObject *parent = nullptr);

public Q_SLOTS:
    /// @brief Get authentication token for user
    /// @param username user name
    /// @param password user password
    /// @return token on success, otherwise error.
    ///
    /// Response example:
    /// @code{.json}
    /// {
    ///     "id": 503,
    ///     "jsonrpc": "2.0",
    ///     "result": {
    ///         "token": "kghpUjs0cBVI917ZMxHPYB5lSy59dQim",
    ///         "user": {
    ///             "username": "admin"
    ///         }
    ///     }
    /// }
    /// @endcode
    /// Error example:
    /// @code{.json}
    /// {
    ///     "error": {
    ///         "code": -32603,
    ///         "data": null,
    ///         "message": "Invalid username or password"
    ///     },
    ///     "id": 1,
    ///     "jsonrpc": "2.0"
    /// }
    /// @endcode
    QJsonObject login(const QString &username, const QString &password);

    /// @brief Logout user
    /// @param token authentication token
    /// @return true if success(false if token not found).
    ///
    /// Response example:
    /// @code{.json}
    /// {
    ///     "id": 503,
    ///     "jsonrpc": "2.0",
    ///     "result": true
    /// }
    /// @endcode
    bool logout(const QString &token);

    /// @brief Check authentication token
    /// @param token authentication token
    /// @return true if success(false if token not found).
    ///
    /// Response example:
    /// @code{.json}
    /// {
    ///     "id": 503,
    ///     "jsonrpc": "2.0",
    ///     "result": true
    /// }
    /// @endcode
    bool checkAuth(const QString &token);

    /// @brief Get user identity
    /// @param token authentication token
    /// @return user identity.
    ///
    /// Response example:
    /// @code{.json}
    /// {
    ///     "id": 503,
    ///     "jsonrpc": "2.0",
    ///     "result": {
    ///         "username": "admin"
    ///     }
    /// }
    /// @endcode
    /// Error example:
    /// @code{.json}
    /// {
    ///     "error": {
    ///         "code": -32602,
    ///         "data": null,
    ///         "message": "Invalid token"
    ///     },
    ///     "id": 1,
    ///     "jsonrpc": "2.0"
    /// }
    /// @endcode
    QJsonObject getIdentity(const QString &token);

private:
    std::vector<std::unique_ptr<IUserStorage> > users;

    std::unique_ptr<IAuthStorage> auths;

    QString secret, name;
};


#endif
