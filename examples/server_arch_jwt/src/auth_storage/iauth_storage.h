#ifndef IAUTH_STORAGE_H
#define IAUTH_STORAGE_H

#include <optional>
#include <utility>
#include <QString>
#include <QPair>

class IAuthStorage {
public:
    /// @brief create internal authentication identifier
    /// @param username user name
    /// @param userVersion user version
    /// @return authentication identifier
    [[nodiscard]] virtual QString authenticate(const QString &username, const QString &userVersion) = 0;

    /// @brief get user data by authentication identifier
    /// @param auth_id authentication identifier
    /// @return username and user version on success, or std::nullopt
    [[nodiscard]] virtual std::optional<QPair<QString, QString> > get(const QString &auth_id) = 0;

    /// @brief remove authentication identifier
    /// @param auth_id authentication identifier to remove
    virtual bool remove(const QString &auth_id) = 0;

    virtual ~IAuthStorage() = default;
};

#endif // IAUTH_STORAGE_H
