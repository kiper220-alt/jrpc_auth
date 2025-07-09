#include <iostream>
#include <QtCore>
#include <auth_service.h>
#include <qjsonrpc/qjsonrpchttpserver.h>
#include <user_storage/psql_user_storage.h>
#include <auth_storage/mem_auth_storage.h>

int main(int argc, char* argv[]) {
    QLoggingCategory::setFilterRules("default.debug=true\n"
        "qt.*.debug=true\n"
        "*.debug=true\n"
        "*.info=true\n"
        "*.warning=true\n"
        "*.critical=true");

    QCoreApplication app(argc, argv);

    QJsonRpcHttpServer rpcServer;
    AuthServiceSettings authSettings;

    authSettings.authStorage = std::make_unique<MemAuthStorage>();
    authSettings.userStorages.emplace_back(std::move(std::make_unique<PsqlUserStorage>()));
    
    rpcServer.addService(new AuthService(std::move(authSettings), &rpcServer));
    if (!rpcServer.listen(QHostAddress::LocalHost, 7777)) {
        std::cerr << "Failed to start server" << std::endl;
        return 1;
    }

    return app.exec();
}