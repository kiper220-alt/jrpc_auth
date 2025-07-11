#ifndef MEM_AUTH_STORAGE_H
#define MEM_AUTH_STORAGE_H

#include <auth_storage/iauth_storage.h>
#include <random>
#include <QHash>

class MemAuthStorage : public IAuthStorage {
    QHash<QString, QPair<QString, QString> > token2user;
    std::mt19937 gen;

public:
    /// @brief constructor
    /// @param seed random generator seed
    explicit MemAuthStorage(uint64_t seed = -1);

    /// @brief create internal authentication identifier
    /// @param username user name
    /// @param userVersion user version
    /// @return authentication identifier
    [[nodiscard]] QString authenticate(const QString &username, const QString &userVersion) override;

    /// @brief get user data by authentication identifier
    /// @param auth_id authentication identifier
    /// @return username and user version on success, or std::nullopt
    [[nodiscard]] std::optional<QPair<QString, QString> > get(const QString &auth_id) override;

    /// @brief remove authentication identifier
    /// @param auth_id authentication identifier to remove
    bool remove(const QString &auth_id) override;
};

#endif // MEM_AUTH_STORAGE_H
