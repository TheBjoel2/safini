#include "StringLiteral.hpp"
#include "RxiIniReader.hpp"
#include "AnyTypeStorage.hpp"
#include "TypeHash.cpp"
#include "PairHash.cpp"
#include <string_view>
#include <optional>
#include <unordered_map>
#include <vector>
#include <functional>

namespace safini
{
/////////////////////////////////////
/// \brief Config reader class
///
/// This class reads config file you specified,
/// storing all the constructed parameters that you requested,
/// throwing exceptions if the requested parameter
/// is absolutely required to continue code execution
/////////////////////////////////////
template<const StringLiteral configName>
class Config
{
public:
    /////////////////////////////////////
    /// \brief Constructs required parameters
    ///
    /// Loads config file into memory, finds all the required keys,
    /// extracts values and passes them to object constructors.
    /// Stores the constructed objects inside of itself
    ///
    /// \param configName UNIQUE name of the config. Not necessarily a config filename
    /// \param filename Runtime defined config filename
    ///
    /// \see extract extractOr tryExtract
    /////////////////////////////////////
    Config(const std::string_view filename);

    /////////////////////////////////////
    /// \brief Extracts value, guaranteed to succeed
    ///
    /// Extracts value stored in Config class's internal storage.
    /// Never fails because if you reached this line,
    /// the Config class instance is guaranteed to have your value read and constructed.
    /// Unless there's no undefined behavior
    ///
    /// \param ReturnType Type of the argument(int, string, etc)
    /// \param key String literal of the config key("foo.bar")
    ///
    /// \return const reference to object stored in Config instance
    /////////////////////////////////////
    template<typename ReturnType,
             const StringLiteral key>
    auto extract() const noexcept
    -> const ReturnType&;

    /////////////////////////////////////
    /// \brief Extracts value, otherwise returns fallback value
    ///
    /// Extracts value stored in Config class's internal storage.
    /// In case the provided key is not present in a config file,
    /// or passing it to a function constructing the object failed,
    /// it shall return the fallback value
    ///
    /// \param ReturnType Type of the argument(int, string, etc)
    /// \param key String literal of the config key("foo.bar")
    /// \param fallbackValue Const reference to fallback value
    ///
    /// \return either object present in a Config, or fallbackValue
    /////////////////////////////////////
    template<typename ReturnType,
             const StringLiteral key>
    auto extractOr(const ReturnType& fallbackValue) const noexcept
    -> const ReturnType&;

    /////////////////////////////////////
    /// \brief Extracts value, returns optional of the result
    ///
    /// Returns optional object that may contain value,
    /// or contain nothing in case of the provided key is not present in a config file,
    /// or passing it to a function constructing the object failed
    ///
    /// \param ReturnType Type of the config value(int, string, etc)
    /// \param key String literal of the config key("foo.bar")
    ///
    /// \return std::optional of an object
    /////////////////////////////////////
    template<typename ReturnType,
             const StringLiteral key>
    auto tryExtract() const noexcept
    -> std::optional<std::reference_wrapper<const ReturnType>>;

private:
    RxiIniReader m_IniReader;

    using Key = std::pair<std::string_view, TypeHash>;
    using Storage = AnyTypeStorage;
    std::unordered_map<Key, Storage, PairHash> m_KeysMap{};
};

}

#include "Config.cpp"
