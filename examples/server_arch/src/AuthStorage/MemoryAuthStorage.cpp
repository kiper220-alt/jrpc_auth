
#include <AuthStorage/MemoryAuthStorage.h>

static std::array<char, 16> generateToken(std::mt19937_64 &randomEngine) {
    constexpr std::string_view possibleCharacters = ("1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
    std::uniform_int_distribution<> distribution(0, possibleCharacters.length() - 1);

    std::array<char, 16> token{};
    for (auto &c: token) {
        c = possibleCharacters[distribution(randomEngine)];
    }
    return token;
}

MemoryAuthStorage::MemoryAuthStorage(int64_t seed) : randomEngine(seed) {
    if (seed == -1) {
        randomEngine.seed(std::random_device()());
    }
}

QString MemoryAuthStorage::authenticate(const QString &username) {
    QString token;
    do {
        token = QString::fromLatin1(generateToken(this->randomEngine).data(), 16);
    } while (this->token2user.contains(token));

    this->token2user[token] = username;
    if (const auto it = this->user2token.find(username); it == this->user2token.end()) {
        this->user2token[username] = QStringList{token};
    } else {
        it.value().append(token);
    }

    return token;
}

std::optional<QString> MemoryAuthStorage::getUserName(const QString &token) {
    if (const auto it = this->token2user.find(token); it != this->token2user.end()) {
        return it.value();
    }
    return std::nullopt;
}

void MemoryAuthStorage::removeToken(const QString &token) {
    if (const auto it = this->token2user.find(token); it != this->token2user.end()) {
        this->user2token[it.value()].removeOne(token);
        this->token2user.remove(token);
    }
}

void MemoryAuthStorage::removeUser(const QString &username) {
    if (const auto it = this->user2token.find(username); it != this->user2token.end()) {
        for (const auto &token: it.value()) {
            this->token2user.remove(token);
        }
        this->user2token.remove(username);
    }
}
