#include <optional>
#include <utility>
#include <QString>

class IAuthStorage {
public:
    /// @brief create internal authentication identifier
    /// @param username user name
    /// @param userVersion user version
    /// @return authentication identifier
    virtual uint64_t authenticate(const QString &username, uint64_t userVersion) = 0;
    
    /// @brief get user data by authentication identifier
    /// @param auth_id authentication identifier
    /// @return username and user version on success, or std::nullopt
    virtual std::optional<const std::pair<QString, uint64_t>&> get(uint64_t auth_id) = 0;
    
    /// @brief remove authentication identifier
    /// @param auth_id authentication identifier to remove
    virtual void remove(uint64_t auth_id) = 0;

    virtual ~IAuthStorage() = default;
};