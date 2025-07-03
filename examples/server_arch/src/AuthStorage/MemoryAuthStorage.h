
#ifndef MEMORYAUTHSTORAGE_H
#define MEMORYAUTHSTORAGE_H
#include <common.h>
#include <random>


class MemoryAuthStorage : public IAuthStorage {
private:
    QHash<QString, QString> token2user;
    QHash<QString, QStringList> user2token;
    std::mt19937_64 randomEngine;

public:
    MemoryAuthStorage(int64_t seed = -1);

    /**
     * @param username username
     * @return auth token
     */
    virtual QString authenticate(const QString &username) override;

    /**
     * @param token auth token
     * @return user name
     */
    virtual std::optional<QString> getUserName(const QString &token) override;

    /**
     * @param token auth token to remove
     */
    virtual void removeToken(const QString &token) override;

    /**
     * @param username to remove all auth tokens associated with
     */
    void removeUser(const QString &username) override;

    virtual ~MemoryAuthStorage() = default;
};

#endif //MEMORYAUTHSTORAGE_H
