#include <QtCore>
#include <service/auth_service.h>
#include <qjsonrpc/qjsonrpchttpserver.h>
#include <user_storage/qsql_user_storage.h>
#include <auth_storage/mem_auth_storage.h>
#include <auth_configuration/json_configuration.h>

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    QJsonRpcHttpServer rpcServer;
    AuthServiceSettings authSettings;
    JsonConfiguration configuration = loadConfiguration();

    authSettings.authStorage = std::make_unique<MemAuthStorage>();
    authSettings.userStorages.emplace_back(std::move(std::make_unique<QSqlUserStorage>(&configuration)));

    rpcServer.addService(new AuthService(std::move(authSettings), &configuration, &rpcServer));
    if (!rpcServer.listen(QHostAddress::LocalHost, 7777)) {
        qDebug() << "Failed to start Json-RPC HTTP server";
        qDebug() << rpcServer.errorString();
        return 1;
    }

    return app.exec();
}
