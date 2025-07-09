#include <optional>
#include <QString>

class IUserStorage {
public:
    /// @brief Just authenticate
    /// @param username authentication user name 
    /// @param password authentication password
    /// @return authentication version if success (can be hash of user data), otherwise std::nullopt.
    virtual std::optional<QString> authenticate(const QString &username, const QString &password) = 0;

    /// @brief Just get user version
    /// @param username user name
    /// @return user version if user exists, otherwise std::nullopt
    virtual std::optional<QString> getUserVersion(const QString &username) = 0;
    
    virtual ~IUserStorage() = default;
};