#include <auth_storage/mem_auth_storage.h>

static QString randomToken(std::mt19937 &gen) {
    const static std::string chars = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::uniform_int_distribution<> dis(0, static_cast<int>(chars.size() - 1));
    std::string token;
    for (int i = 0; i < 32; ++i) {
        token.push_back(chars[dis(gen)]);
    }
    return QString::fromStdString(token);
}

MemAuthStorage::MemAuthStorage(const uint64_t seed) : gen(seed) {
    if (seed == -1) {
        this->gen.seed(std::random_device{}());
    }
}

QString MemAuthStorage::authenticate(const QString &username, const QString &userVersion) {
    QString token = randomToken(this->gen);
    while (this->token2user.contains(token)) {
        token = randomToken(this->gen);
    }
    this->token2user[token] = {username, userVersion};
    return token;
}

std::optional<QPair<QString, QString> > MemAuthStorage::get(const QString &auth_id) {
    if (this->token2user.contains(auth_id)) {
        return this->token2user[auth_id];
    }
    return std::nullopt;
}

bool MemAuthStorage::remove(const QString &auth_id) {
    return this->token2user.remove(auth_id);
}
