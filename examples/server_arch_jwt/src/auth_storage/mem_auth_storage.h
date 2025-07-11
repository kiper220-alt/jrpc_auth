#ifndef MEM_AUTH_STORAGE_H
#define MEM_AUTH_STORAGE_H

#include <auth_storage/iauth_storage.h>
#include <random>
#include <QHash>

class MemAuthStorage : public IAuthStorage {
    QHash<QString, QPair<QString, QString> > token2user;
    std::mt19937 gen;

public:
    explicit MemAuthStorage(uint64_t seed = -1);

    [[nodiscard]] QString authenticate(const QString &username, const QString &userVersion) override;

    [[nodiscard]] std::optional<QPair<QString, QString> > get(const QString &auth_id) override;

    bool remove(const QString &auth_id) override;
};

#endif // MEM_AUTH_STORAGE_H
