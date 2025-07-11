#include <user_storage/psql_user_storage.h>
#include <QtSql/qsqlquery.h>
#include <QtSql/qsqldriver.h>
#include <QtSql/qsqlerror.h>
#include <QCryptographicHash>
#include <QVariant>
#include <QDebug>

#define DO_IF_NOT_EMPTY(x, y) if (!x.isEmpty()) { y(x); }
#define DO_ELSE(x, y) else { y(x); }
#define DO_IF_NOT_EMPTY_AS_INT(x, y) if (!x.isEmpty()) { y(x.toInt()); }

const std::string &getPasswordSalt() {
    const static auto _salt = std::getenv("SOME_PASSWORD_SALT"); // TODO: move to config
    const static std::string salt = _salt ? _salt : "SOME_PASSWORD_SALT";
    return salt;
}

const QCryptographicHash::Algorithm &getHashAlgorithm() {
    const static QCryptographicHash::Algorithm algorithm = QCryptographicHash::Sha256; // TODO: move to config
    return algorithm;
}

static QString computePasswordHash(const QString &username, const QString &password) {
    const QCryptographicHash::Algorithm algorithm = getHashAlgorithm();

    if (QCryptographicHash::hashLength(algorithm) > 512) {
        throw std::runtime_error("Hash length is too long");
    }

    QCryptographicHash hash(algorithm);

    hash.addData(getPasswordSalt().c_str(), getPasswordSalt().size());
    hash.addData(username.toUtf8());
    hash.addData(password.toUtf8());

    return QString::fromUtf8(hash.result().toHex()); // max 1024 bytes
}

/// @brief Default constructor
PsqlUserStorage::PsqlUserStorage() {
    QString host = std::getenv("DATABASE_HOST");
    QString driver = std::getenv("DATABASE_DRIVER");
    QString port = std::getenv("DATABASE_PORT");
    QString name = std::getenv("DATABASE_NAME");
    QString user = std::getenv("DATABASE_USER");
    QString password = std::getenv("DATABASE_PASSWORD");
    this->schema = std::getenv("DATABASE_SCHEMA");

    if (driver.isEmpty()) {
        driver = "QPSQL";
    }
    if (name.isEmpty()) {
        name = "users";
    }
    if (this->schema.isEmpty()) {
        this->schema = "public";
    }
    if (user.isEmpty()) {
        user = qgetenv("USER");
        if (user.isEmpty()) {
            user = qgetenv("USERNAME");
        }
    }

    db = QSqlDatabase::addDatabase(driver, name);

    DO_IF_NOT_EMPTY_AS_INT(port, db.setPort);
    DO_IF_NOT_EMPTY(host, db.setHostName);
    DO_IF_NOT_EMPTY(user, db.setUserName);
    DO_IF_NOT_EMPTY(password, db.setPassword);
    db.setDatabaseName(name);

    if (!db.open()) {
        throw std::runtime_error(db.lastError().text().toStdString());
    }
}

std::optional<QString> PsqlUserStorage::authenticate(const QString &username, const QString &password) {
    QSqlQuery query(db);
    const QString hashed = computePasswordHash(username, password);
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

std::optional<QString> PsqlUserStorage::getUserVersion(const QString &username) {
    QSqlQuery query(db);
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
