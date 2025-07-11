#ifndef PSQL_USER_STORAGE_H
#define PSQL_USER_STORAGE_H

#include <user_storage/iuser_storage.h>
#include <QtSql/qsqldatabase.h>

/// @brief QSqlUserStorage
/// Automatically connect to database. If connection fails, throw exception.
/// parameters from environment:
/// - DATABASE_HOST: database host address (default - local socket)
/// - DATABASE_SCHEMA: database schema (default - "public")
/// - DATABASE_DRIVER: database driver name (default - "QPSQL")
/// - DATABASE_PORT: database port (default - depend on driver)
/// - DATABASE_NAME: database name (default - "users")
/// - DATABASE_USER: database user name (default - ${USER}|${USERNAME}, then default of driver)
/// - DATABASE_PASSWORD: database user password (default - empty)
/// Hash of pasword built from `HASH_OF(SALT ~ USER ~ PASSWORD)`, where `~` - concatenation operator.
/// 
class QSqlUserStorage : public IUserStorage {
private:
    QSqlDatabase db;
    QString schema;

public:
    QSqlUserStorage();

    /// @brief Just authenticate
    /// @param username authentication user name 
    /// @param password authentication password
    /// @return authentication version if success (can be hash of user data), otherwise std::nullopt.
    [[nodiscard]] std::optional<QString> authenticate(const QString &username, const QString &password) override;

    /// @brief Just get user version
    /// @param username user name
    /// @return user version if user exists, otherwise std::nullopt
    [[nodiscard]] std::optional<QString> getUserVersion(const QString &username) override;

    ~QSqlUserStorage() = default;
};

#endif
