
#ifndef AUTHSERVICE_H
#define AUTHSERVICE_H

#include <common.h>
#include <qjsonrpc/qjsonrpcservice.h>

class AuthService : public QJsonRpcService {
public:
    Q_OBJECT
    Q_CLASSINFO("serviceName", "auth")

    AuthService(const AuthService &) = delete;

    explicit AuthService(std::unique_ptr<IAuthStorage> storage, QObject *parent = nullptr);

    QVariantMap login(const QString &username, const QString &password);

    void logout(const QString &token);

    bool checkAuth(const QString &token);

    QVariantMap getIdentity(const QString &token);

    friend void addUserStorage(AuthService &authService, std::unique_ptr<IUserStorage> userStorage);

private:
    std::vector<std::unique_ptr<IUserStorage> > authStorages;

    std::unique_ptr<IAuthStorage> storage;
};


#endif //AUTHSERVICE_H
