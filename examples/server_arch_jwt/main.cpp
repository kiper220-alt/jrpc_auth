#include <iostream>
#include <QtCore>
#include <auth_service.h>
#include <qjsonrpc/qjsonrpchttpserver.h>
#include <user_storage/psql_user_storage.h>
#include <auth_storage/mem_auth_storage.h>

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);
    QJsonRpcHttpServer rpcServer;
    AuthServiceSettings authSettings;
    authSettings.authStorage = std::make_unique<MemAuthStorage>();
    authSettings.userStorages.emplace_back(std::move(std::make_unique<PsqlUserStorage>()));
    rpcServer.addService(new AuthService(std::move(authSettings), &rpcServer));
    rpcServer.listen(QHostAddress::Any, 7777);

    return 0;
}