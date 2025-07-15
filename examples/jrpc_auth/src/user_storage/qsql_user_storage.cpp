#include <user_storage/qsql_user_storage.h>
#include <QtSql/qsqlquery.h>
#include <QtSql/qsqldriver.h>
#include <QtSql/qsqlerror.h>
#include <QCryptographicHash>
#include <QVariant>
#include <QDebug>

#define DO_IF_NOT_EMPTY(x, y) if (!x.isEmpty()) { y(x); }
#define DO_ELSE(x, y) else { y(x); }
#define DO_IF_NOT_EMPTY_AS_INT(x, y) if (!x.isEmpty()) { y(x.toInt()); }

#define SET_FROM_CONFIG(var, config, option) \
    do { \
        QString str; \
        bool isValid = false; \
        if (config) { \
            const QVariant value = config->getUserConfig(option); \
            str = value.toString(); \
            isValid = value.isValid() && !str.isEmpty() && str != QLatin1String("default"); \
            if (isValid) { \
                var = str; \
            } \
        } \
        qDebug().noquote() << "QSqlUserStorage:" << option << "=" << (isValid ? var : "default"); \
    } while (false)

#define SET_FROM_CONFIG_OR(var, config, option, default_value) \
    do { \
        QString varValue = default_value; \
        if (config) { \
            const QVariant value = config->getUserConfig(option); \
            QString str = value.toString(); \
            if (value.isValid() && !str.isEmpty() && str != QLatin1String("default")) { \
                varValue = str; \
            } \
        } \
        var = std::move(varValue); \
        qDebug().noquote() << "QSqlUserStorage:" << option << "=" << var; \
    } while (false)

const QCryptographicHash::Algorithm &getHashAlgorithm() {
    const static QCryptographicHash::Algorithm algorithm = QCryptographicHash::Sha256; // TODO: move to config
    return algorithm;
}

static QString computePasswordHash(const QString &password, const QString &salt) {
    const QCryptographicHash::Algorithm algorithm = getHashAlgorithm();

    if (QCryptographicHash::hashLength(algorithm) > 512) {
        throw std::runtime_error("Hash length is too long");
    }

    QCryptographicHash hash(algorithm);

    hash.addData(salt.toUtf8());
    hash.addData(password.toUtf8());

    return QString::fromUtf8(hash.result().toHex()); // max 1024 bytes
}

/// @brief Default constructor
QSqlUserStorage::QSqlUserStorage(IUserConfig *config) {
    QString host;
    QString driver;
    QString port;
    QString name;
    QString user;
    QString password;
    this->schema;

    /// Compiler will optimise `if (config)` in release build.
    SET_FROM_CONFIG(host, config, "host");
    SET_FROM_CONFIG(port, config, "port");
    SET_FROM_CONFIG(user, config, "user");
    SET_FROM_CONFIG_OR(this->schema, config, "schema", "public");
    SET_FROM_CONFIG(password, config, "password");
    SET_FROM_CONFIG_OR(driver, config, "driver", "qpsql");
    SET_FROM_CONFIG_OR(name, config, "name", "users");
    SET_FROM_CONFIG_OR(this->salt, config, "salt", "SOME_PASSWORD_SALT");

    /// qpsql -> QPSQL
    driver = driver.toUpper();

    this->db = QSqlDatabase::addDatabase(driver, name);

    DO_IF_NOT_EMPTY_AS_INT(port, this->db.setPort);
    DO_IF_NOT_EMPTY(host, this->db.setHostName);
    DO_IF_NOT_EMPTY(user, this->db.setUserName);
    DO_IF_NOT_EMPTY(password, this->db.setPassword);
    this->db.setDatabaseName(name);

    if (!db.open()) {
        throw std::runtime_error(this->db.lastError().text().toStdString());
    }
}

std::optional<QString> QSqlUserStorage::authenticate(const QString &username, const QString &password) {
    QSqlQuery query(this->db);
    const QString hashed = computePasswordHash(password, this->salt);
    const QString safeTable = this->db.driver()->escapeIdentifier(this->schema + ".users", QSqlDriver::TableName);

    query.prepare("SELECT password FROM " + safeTable + " WHERE username = :username");

    query.bindValue(":username", username);

    if (query.exec()) {
        if (query.next()) {
            if (query.value(0).toString() == hashed) {
                return query.value(0).toString();
            } else {
                qDebug() << "Password does not match for user:" << username;
                qDebug() << "Hash in the database:" << query.value(0).toString() << " Provided hash:" << hashed;
            }
        } else {
            qDebug() << "User not found:" << username;
        }
    } else {
        qDebug() << "Error executing request:" << query.lastError().text();
    }

    qDebug() << "Request completed:" << query.lastQuery();
    qDebug() << "Associated values:" << query.boundValues();

    return std::nullopt;
}

std::optional<QString> QSqlUserStorage::getUserVersion(const QString &username) {
    QSqlQuery query(this->db);
    const QString safeTable = this->db.driver()->escapeIdentifier(this->schema + ".users", QSqlDriver::TableName);

    query.prepare("SELECT password FROM " + safeTable + " WHERE username = :username");

    query.bindValue(":username", username);

    if (query.exec()) {
        if (query.next()) {
            return query.value(0).toString();
        } else {
            qDebug() << "User not found:" << username;
        }
    } else {
        qDebug() << "Error executing request:" << query.lastError().text();
    }

    qDebug() << "Request completed:" << query.lastQuery();
    qDebug() << "Associated values:" << query.boundValues();

    return std::nullopt;
}
