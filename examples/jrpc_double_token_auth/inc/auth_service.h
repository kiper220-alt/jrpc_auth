#ifndef AUTH_SERVICE_H
#define AUTH_SERVICE_H

#include <qjsonrpc/qjsonrpcservice.h>
#include <auth_storage/iauth_storage.h>
#include <user_storage/iuser_storage.h>
#include <auth_configuration/iservice_config.h>

typedef struct AuthServiceSettings {
    std::unique_ptr<IAuthStorage> authStorage;
    std::vector<std::unique_ptr<IUserStorage> > userStorages;

    ~AuthServiceSettings() = default;
} AuthServiceSettings;

class AuthService : public QJsonRpcService {
    Q_OBJECT
    Q_CLASSINFO("serviceName", "auth")

public:
    AuthService(const AuthService &) = delete;

    /// @brief Constructor
    /// @param settings authentication settings
    explicit AuthService(AuthServiceSettings &&settings, const IServiceConfig *config = nullptr,
                         QObject *parent = nullptr);

public Q_SLOTS:
    /// @brief Create new authentication token pair (access and refresh)
    /// @param username user name
    /// @param password user password
    /// @param audience service name to create token
    /// @return tokens on success, otherwise error.
    /// Token - JWT token, that contains "iss"(username), "sub"(service name), "jti"(token id) and "exp"(expiration date).
    ///
    /// Response example:
    /// @code{.json}
    /// {
    ///     "id": 1,
    ///     "jsonrpc": "2.0",
    ///     "result": {
    ///         "access": "eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9.eyJhdWQiOiJTb21lU2VydmljZSIsImV4cCI6MTc1Mjc0Mzk0NywiaWF0IjoxNzUyNzQzNjQ3LCJpc3MiOiJhdXRoIiwianRpIjoialh6c0hkR25VU3oyUkJNYWJvRzJIcEZRYTluQWpuMkciLCJuYmYiOjE3NTI3NDM2NDcsInJlZiI6ZmFsc2UsInN1YiI6ImFkbWluIn0.itTp8S1U5RXQcdYebnIxbCjAzicHdILW0PFJ7vz0njq7cBO1macIeKahP17MwTtF3I5RZ5XuSWEHwDNi4TpIEoV2ORSAxMfKLUuy7CMk7W4UtQisEYuNOHRb5P1kKxySTlx3bpWYh1qjeCRe1K-G9fzQVY0Yv-TYgQcx-HOFV-De5dTMiqphdSjO1P1Pwta2gB2huHQDpaOAmrNsNJ7vD5Tnavd-wAoo_HCca0RbygEdsZJSTAiKP-5qHHsgAS2wWIi-lYiHIAALmkEx5gKZHBlyzfH8I76X-8LT3a4O0gqdIHGqmZzEJK9hH1pRO8-NgXnIXel-phGhss-bYzyzKm0iwku8ioRbn0V49GEkMc4JhyHIiTANZ3ADozzloMFoa9qC71_ufHgp_3rXNE-CnwzJD-V4S3NWzQCraW-LIKud99pmOhW0OxVX0xdGsD5TqU4ELdanCu5LZt1HbK1lbjoFcfB20XIu7OgbFxcMz-htfNTnVkza_55AEOyB9yiS1z2sOwdYxNg58RsJD4xNcKveDa-_U0o7eU1EmIXAXrOlJJTyAy633uHFRVjFplkqV4hxiK5fdJ1iz02SUZVW33UUkcV-MAnWaljzjmCwT3wl0P49NmbmwI0k6FUaAxdUmWIdNALXlJuFA_RX5PfU1oaKZ0FAgd3-fPjw1V8iHT8"
    ///         ,
    ///         "refresh": "eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9.eyJhdWQiOiJTb21lU2VydmljZSIsImV4cCI6MTc1MjgzMDA0NywiaWF0IjoxNzUyNzQzNjQ3LCJpc3MiOiJhdXRoIiwianRpIjoialh6c0hkR25VU3oyUkJNYWJvRzJIcEZRYTluQWpuMkciLCJuYmYiOjE3NTI3NDQyNDcsInJlZiI6dHJ1ZSwic3ViIjoiYWRtaW4ifQ.kYUPP-fBfsvkxY2dC8nTvUdIev6fZUQrZGW1Sgqt855HLq_CK99Zj2M0Sov2rbmrXevQ0-UXMyxaC1OlmR_uqGZfQmoUoJl-5gHxwFugUeW2iSwEOhr6r5VY-LBwkI7jroN6gNd9yDtrNfPiyZRMF_QRcTYOdGaGjQoa4NchSksg0dc79d0smRSpsiLR-N-8Mh_c1gsNo7w8NQEWvhOxN0eLtvfDnf_M4FA0ENlWwYJfH27n9H_-90X9S5zl0G9eMEC6kyGVd6df-9Lqm_uLIemWHHPLbdSRJNBTtjm_lhb-JRmJ-TkThU84hdU-ULW3EE5IPsS5S4pY0POIRHaszv5IjGh77-Vt-XNSUz_v0vkyR7LjxyMrZFGfDu8y3XnS2KC1UOw2_iPs4on2nqwF-I8MB0KDWZEWOhXXlbvSVt66FLGbY-ouaU-PhLAyL5G2f6LCVH21v59rn5rmVAQ-cTs4N8kPRiVo6f9PUJTa9f4IkAqPhIAtYyIcW6_JlhmTAwBACItC0a2rxoXUjmsRId22GxiLRoB3YLh5NuLs4mMAZqNbnIaO42WiAZVWTFycsFfgLCu6QXbEK_NXLKHfExr0j2RCQkTr2hyyBtMdUgh6H8nqWuqKiKIOBGsZ_R-SxojRyvcyXIjyz20okG8WBZaBAM5vQKhVtN1W6TZNB5Y"
    ///         ,
    ///         "user": {
    ///             "username": "admin"
    ///         }
    ///     }
    /// }
    /// @endcode
    /// Error example:
    /// @code{.json}
    /// {
    ///     "error": {
    ///         "code": -32603,
    ///         "data": null,
    ///         "message": "Invalid username or password"
    ///     },
    ///     "id": 1,
    ///     "jsonrpc": "2.0"
    /// }
    /// @endcode
    QJsonObject login(const QString &username, const QString &password, const QString &audience);

    /// @brief Refresh authentication token
    /// @param token authentication refresh token
    /// @return new token pair on success, otherwise error.
    /// Token - JWT token, that contains "iss"(username), "sub"(service name), "jti"(token id) and "exp"(expiration date).
    /// If it's refresh token, added field "ajti" (access token id) and "exp"(expiration date) at least 1 day.
    /// After refresh, old access and refresh tokens will be invalid.
    ///
    /// Response example:
    /// @code{.json}
    /// {
    ///     "id": 1,
    ///     "jsonrpc": "2.0",
    ///     "result": {
    ///         "access": "eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9.eyJhdWQiOiJTb21lU2VydmljZSIsImV4cCI6MTc1Mjc0Mzk0NywiaWF0IjoxNzUyNzQzNjQ3LCJpc3MiOiJhdXRoIiwianRpIjoialh6c0hkR25VU3oyUkJNYWJvRzJIcEZRYTluQWpuMkciLCJuYmYiOjE3NTI3NDM2NDcsInJlZiI6ZmFsc2UsInN1YiI6ImFkbWluIn0.itTp8S1U5RXQcdYebnIxbCjAzicHdILW0PFJ7vz0njq7cBO1macIeKahP17MwTtF3I5RZ5XuSWEHwDNi4TpIEoV2ORSAxMfKLUuy7CMk7W4UtQisEYuNOHRb5P1kKxySTlx3bpWYh1qjeCRe1K-G9fzQVY0Yv-TYgQcx-HOFV-De5dTMiqphdSjO1P1Pwta2gB2huHQDpaOAmrNsNJ7vD5Tnavd-wAoo_HCca0RbygEdsZJSTAiKP-5qHHsgAS2wWIi-lYiHIAALmkEx5gKZHBlyzfH8I76X-8LT3a4O0gqdIHGqmZzEJK9hH1pRO8-NgXnIXel-phGhss-bYzyzKm0iwku8ioRbn0V49GEkMc4JhyHIiTANZ3ADozzloMFoa9qC71_ufHgp_3rXNE-CnwzJD-V4S3NWzQCraW-LIKud99pmOhW0OxVX0xdGsD5TqU4ELdanCu5LZt1HbK1lbjoFcfB20XIu7OgbFxcMz-htfNTnVkza_55AEOyB9yiS1z2sOwdYxNg58RsJD4xNcKveDa-_U0o7eU1EmIXAXrOlJJTyAy633uHFRVjFplkqV4hxiK5fdJ1iz02SUZVW33UUkcV-MAnWaljzjmCwT3wl0P49NmbmwI0k6FUaAxdUmWIdNALXlJuFA_RX5PfU1oaKZ0FAgd3-fPjw1V8iHT8",
    ///         "refresh": "eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9.eyJhdWQiOiJTb21lU2VydmljZSIsImV4cCI6MTc1MjgzMDA0NywiaWF0IjoxNzUyNzQzNjQ3LCJpc3MiOiJhdXRoIiwianRpIjoialh6c0hkR25VU3oyUkJNYWJvRzJIcEZRYTluQWpuMkciLCJuYmYiOjE3NTI3NDQyNDcsInJlZiI6dHJ1ZSwic3ViIjoiYWRtaW4ifQ.kYUPP-fBfsvkxY2dC8nTvUdIev6fZUQrZGW1Sgqt855HLq_CK99Zj2M0Sov2rbmrXevQ0-UXMyxaC1OlmR_uqGZfQmoUoJl-5gHxwFugUeW2iSwEOhr6r5VY-LBwkI7jroN6gNd9yDtrNfPiyZRMF_QRcTYOdGaGjQoa4NchSksg0dc79d0smRSpsiLR-N-8Mh_c1gsNo7w8NQEWvhOxN0eLtvfDnf_M4FA0ENlWwYJfH27n9H_-90X9S5zl0G9eMEC6kyGVd6df-9Lqm_uLIemWHHPLbdSRJNBTtjm_lhb-JRmJ-TkThU84hdU-ULW3EE5IPsS5S4pY0POIRHaszv5IjGh77-Vt-XNSUz_v0vkyR7LjxyMrZFGfDu8y3XnS2KC1UOw2_iPs4on2nqwF-I8MB0KDWZEWOhXXlbvSVt66FLGbY-ouaU-PhLAyL5G2f6LCVH21v59rn5rmVAQ-cTs4N8kPRiVo6f9PUJTa9f4IkAqPhIAtYyIcW6_JlhmTAwBACItC0a2rxoXUjmsRId22GxiLRoB3YLh5NuLs4mMAZqNbnIaO42WiAZVWTFycsFfgLCu6QXbEK_NXLKHfExr0j2RCQkTr2hyyBtMdUgh6H8nqWuqKiKIOBGsZ_R-SxojRyvcyXIjyz20okG8WBZaBAM5vQKhVtN1W6TZNB5Y",
    ///     }
    /// }
    /// @endcode
    /// Error example:
    /// @code{.json}
    /// {
    ///     "error": {
    ///         "code": -32603,
    ///         "data": null,
    ///         "message": "Invalid refresh token"
    ///     },
    ///     "id": 1,
    ///     "jsonrpc": "2.0"
    /// }
    /// @endcode
    QJsonObject refresh(const QString &token);

    /// @brief Logout user
    /// @param token authentication token
    /// @return true if success(false if token not found).
    ///
    /// Response example:
    /// @code{.json}
    /// {
    ///     "id": 503,
    ///     "jsonrpc": "2.0",
    ///     "result": true
    /// }
    /// @endcode
    bool logout(const QString &token);

    /// @brief Check authentication token
    /// @param token authentication token
    /// @return true if success(false if token not found).
    ///
    /// Response example:
    /// @code{.json}
    /// {
    ///     "id": 503,
    ///     "jsonrpc": "2.0",
    ///     "result": true
    /// }
    /// @endcode
    bool checkAuth(const QString &token);

    /// @brief Get user identity
    /// @param token authentication token
    /// @return user identity.
    ///
    /// Response example:
    /// @code{.json}
    /// {
    ///     "id": 503,
    ///     "jsonrpc": "2.0",
    ///     "result": {
    ///         "username": "admin"
    ///     }
    /// }
    /// @endcode
    /// Error example:
    /// @code{.json}
    /// {
    ///     "error": {
    ///         "code": -32602,
    ///         "data": null,
    ///         "message": "Invalid token"
    ///     },
    ///     "id": 1,
    ///     "jsonrpc": "2.0"
    /// }
    /// @endcode
    QJsonObject getIdentity(const QString &token);

private:
    [[nodiscard]] QPair<QString, QString> createTokens(const QString &username, const QString &audience) const;

    [[nodiscard]] std::optional<QPair<QString, QString> > newPairFromRefresh(const QString &refreshToken) const;

private:
    std::vector<std::unique_ptr<IUserStorage> > users;

    std::unique_ptr<IAuthStorage> auths;
    /// @brief Service signing keys
    QString privateKey, publicKey;
    /// @brief Current service name
    QString serviceName;
};


#endif
