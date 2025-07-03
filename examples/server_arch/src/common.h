
#ifndef COMMON_H
#define COMMON_H
#include <qjsonobject.h>
#include <optional>

struct AuthData {
    QString id;
    QVariantMap user;
};


class IUserStorage {
public:
    /**
     * @return current users storage name
     */
    virtual const QString &getName() = 0;

    /**
     * @param username username
     * @param password passw
     * @return Login user and return authentication data on success.
     *
     * Authentication data:
     * - ["id"] - internal user id
     * - ["user"] - any user data
     */
    virtual std::optional<AuthData> authenticate(const QString &username, const QString &password) = 0;

    /**
     * @param token auth token
     * @return user info on success.
     */
    virtual std::optional<QVariantMap> getUserInfo(const QString &id) = 0;

    virtual ~IUserStorage() = default;
};

class IAuthStorage {
public:
    /**
     * @param username username
     * @return auth token
     */
    virtual QString authenticate(const QString &username) = 0;

    /**
     * @param token auth token
     * @return user name
     */
    virtual std::optional<QString> getUserName(const QString &token) = 0;

    /**
     * @param token auth token to remove
     */
    virtual void removeToken(const QString &token) = 0;

    /**
     * @param username to remove all auth tokens associated with
     */
    virtual void removeUser(const QString &username) = 0;

    virtual ~IAuthStorage() = default;
};

#endif //COMMON_H
