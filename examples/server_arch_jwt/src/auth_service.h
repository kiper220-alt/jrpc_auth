#ifndef AUTH_SERVICE_H
#define AUTH_SERVICE_H

#include <qjsonrpc/qjsonrpcservice.h>
#include <auth_storage/iauth_storage.h>
#include <user_storage/iuser_storage.h>

typedef struct AuthServiceSettings {
    std::unique_ptr<IAuthStorage> authStorage;
    std::vector<std::unique_ptr<IUserStorage>> userStorages;

    ~AuthServiceSettings() = default;
} AuthServiceSettings;

class AuthService : public QJsonRpcService {
    Q_OBJECT
    Q_CLASSINFO("serviceName", "auth")

public:
    AuthService(const AuthService &) = delete;
  
    explicit AuthService(AuthServiceSettings &&settings, QObject *parent = nullptr);

public Q_SLOTS:

    QVariantMap login(const QString &username, const QString &password);

    void logout(const QString &token);

    bool checkAuth(const QString &token);

    QVariantMap getIdentity(const QString &token);

private:
    std::vector<std::unique_ptr<IUserStorage> > users;

    std::unique_ptr<IAuthStorage> auths;
};


#endif