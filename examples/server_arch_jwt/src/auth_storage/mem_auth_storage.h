#include <auth_storage/iauth_storage.h>
#include <random>
#include <QHash>

class MemAuthStorage : public IAuthStorage
{
    QHash<QString, QPair<QString, QString>> token2user;
    std::mt19937 gen;
public:
    MemAuthStorage(uint64_t seed = -1);

    QString authenticate(const QString &username, QString userVersion) override;
    std::optional<QPair<QString, QString>> get(QString auth_id) override;
    void remove(QString auth_id) override;
};