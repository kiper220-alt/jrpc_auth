#include <QtCore>
#include <iostream>
#include <qjsonrpc/qjsonrpcservice.h>
/**
 * Start json-rpc service with next api:
 *************************************************************************
 * post `login`:
 * {
 *     "id": $id_of_request
 *     "jsonrpc": "2.0",
 *     "method": "auth.login",
 *     "params": [$user_name, $password],
 * }
 * with response:
 * {
 *     "id": $id_of_request
 *     "jsonrpc": "2.0",
 *     "result": [{
 *         "token": $auth_token,
 *         "user": {
 *             "id": $user_id,
 *             "fullName": $user_name
 *             "avatar": $user_avatar,
 *             "email": $user_avatar,
 *         }
 *     }, $user_name, $password],
 * }
 * fullName, avatar and email are optional
 *************************************************************************\
 * post `logout`:
 * {
 *     "id": $id_of_request
 *     "jsonrpc": "2.0",
 *     "method": "auth.logout",
 *     "params": [$auth_token],
 * }
 * with response:
 * {
 *     "id": $id_of_request
 *     "jsonrpc": "2.0",
 *     "result": $auth_token,
 * }
 *************************************************************************
 * post `checkAuth`:
 * {
 *     "id": $id_of_request
 *     "jsonrpc": "2.0",
 *     "method": "auth.checkAuth",
 *     "params": [$auth_token],
 * }
 * with response:
 * {
 *     "id": $id_of_request
 *     "jsonrpc": "2.0",
 *     "result": [$authenticated, $auth_token],
 * }
 *************************************************************************
 * post `getIdentity`:
 * {
 *     "id": $id_of_request
 *     "jsonrpc": "2.0",
 *     "method": "auth.getIdentity",
 *     "params": [$auth_token],
 * }
 * with response:
 * {
 *     "id": $id_of_request
 *     "jsonrpc": "2.0",
 *     "result": [{
 *         "id": $user_id,
 *         "fullName": $user_name
 *         "avatar": $user_avatar,
 *         "email": $user_avatar,
 *     }, $auth_token],
 * }
 */
class AuthService : public QJsonRpcService {
    Q_OBJECT
    Q_CLASSINFO("serviceName", "auth")

public:
    AuthService(QObject *parent = nullptr): QJsonRpcService(parent) {
    }

public Q_SLOTS:
    QVariantMap login(const QString &username, const QString &password) {
        std::cout << username.toStdString() << " " << password.toStdString() << std::endl;
        QMap<QString, QVariant> map = {
            {"email", "test@domain"},
            {"name", username}
        };
        QMap<QString, QVariant> result = {
            {"token", "1234"},
            {
                "user", map
            }
        };
        return result;
    }

    void logout(const QString &token) {
        std::cout << "logout of token: " << token.toStdString() << std::endl;
    }

    bool checkAuth(const QString &token) {
        std::cout << "check auth of token: " << token.toStdString() << std::endl;
        return true;
    }

    QVariantMap getIdentity(const QString &token) {
        std::cout << "get identity of token: " << token.toStdString() << std::endl;
        QMap<QString, QVariant> map = {
            {"email", "test@domain"},
            {"name", "test"}
        };
        return map;
    }
};

#include "main.moc"

// Функция проверки существования пользователя и аутентификации
bool authenticateUser(const QString &username, const QString &password);

int main(int argc, char *argv[]) {
    qputenv("QT_ASSUME_STDERR_HAS_CONSOLE", "1");
    qputenv("QJSONRPC_DEBUG", "1");
    QLoggingCategory::setFilterRules("default.debug=true\n"
        "qt.*.debug=true\n"
        "*.debug=true\n"
        "*.info=true\n"
        "*.warning=true\n"
        "*.critical=true");
    std::cout << authenticateUser("kiper220", "2569");
    /*
    QCoreApplication a(argc, argv);
    QJsonRpcHttpServer server;
    server.addService(new AuthService);
    if (!server.listen(QHostAddress::Any, 5555)) {
        qDebug() << "Unable to start server";
        return -1;
    }

    return QCoreApplication::exec();*/
}
