#include "StringLiteral.hpp"
#include "RxiIniReader.hpp"
#include "AnyTypeStorage.hpp"
#include "Serialize.hpp"
#include <string_view>
#include <optional>
#include <functional>

namespace safini
{
/////////////////////////////////////
/// \brief Config reader class
///
/// This class reads config file you specified,
/// storing all the constructed parameters that you requested,
/// throwing exceptions if the requested parameter
/// is absolutely required to continue code execution.
/////////////////////////////////////
template<typename ConfigName>
class Config
{
public:
    /////////////////////////////////////
    /// \brief Constructs required parameters
    ///
    /// Loads config file into memory, finds all the required keys,
    /// extracts values and passes them to object constructors.
    /// Stores the constructed objects inside of some internal storage.
    /// The lifetime of these objects ends at the end of the Config lifetime.
    ///
    /// \param ConfigName UNIQUE name of the config. Not necessarily a config filename
    /// \param filename Runtime defined config filename
    ///
    /// \see extract extractOr tryExtract
    ///
    /// \note
    /// Creating two Config instances with the same ConfigName
    /// results in logical errors. Do not do that.
    /////////////////////////////////////
    Config(const std::string_view filename);

    /////////////////////////////////////
    /// \brief Extracts value, guaranteed to succeed
    ///
    /// Extracts value stored in Config class's internal storage.
    /// Never fails because if you reached this line,
    /// the Config class instance is guaranteed to have your value read and constructed.
    /// Unless there's undefined behavior.
    ///
    /// \param ReturnType Type of the argument(int, string, etc)
    /// \param key String literal of the config key("foo.bar")
    /// \param serializeFunc function that converts std::string_view to AnyTypeStorage<ReturnType>
    ///
    /// \return const reference to object stored in Config instance
    /////////////////////////////////////
    template<typename ReturnType,
             const StringLiteral key,
             auto serializeFunc = serialize::getSerizlizeFunc<ReturnType>()>
    auto extract() const noexcept
    -> const ReturnType&;

    /////////////////////////////////////
    /// \brief Extracts value, otherwise returns fallback value
    ///
    /// Extracts value stored in Config class's internal storage.
    /// In case the provided key is not present in a config file,
    /// or passing it to a function constructing the object failed,
    /// it shall return the fallback value.
    ///
    /// \param ReturnType Type of the argument(int, string, etc)
    /// \param key String literal of the config key("foo.bar")
    /// \param serializeFunc function that converts std::string_view to AnyTypeStorage<ReturnType>
    /// \param fallbackValue Const reference to fallback value
    ///
    /// \return either object present in a Config, or fallbackValue
    ///
    /// \note
    /// if you construct a fallbackValue at the line where you call extractOr,
    /// you basically destroy the purpose of it being no-fail guaranteed
    /// unless your constructible object is by itself guaranteed to be constructed.
    /////////////////////////////////////
    template<typename ReturnType,
             const StringLiteral key,
             auto serializeFunc = serialize::getSerizlizeFunc<ReturnType>()>
    auto extractOr(const ReturnType& fallbackValue) const noexcept
    -> const ReturnType&;

    /////////////////////////////////////
    /// \brief Extracts value, returns optional of the result
    ///
    /// Returns optional object that may contain value,
    /// or contain nothing in case of the provided key is not present in a config file,
    /// or passing it to a function constructing the object failed.
    ///
    /// \param ReturnType Type of the config value(int, string, etc)
    /// \param key String literal of the config key("foo.bar")
    /// \param serializeFunc function that converts std::string_view to AnyTypeStorage<ReturnType>
    ///
    /// \return std::optional of an object
    /////////////////////////////////////
    template<typename ReturnType,
             const StringLiteral key,
             auto serializeFunc = serialize::getSerizlizeFunc<ReturnType>()>
    auto tryExtract() const noexcept
    -> std::optional<std::reference_wrapper<const ReturnType>>;

    /////////////////////////////////////
    /// \brief Destroys all the objects in the internal storage
    ///
    /// \note Generally you should not call destructors by yourself
    /////////////////////////////////////
    ~Config();

private:
    RxiIniReader m_IniReader;
};

}

#include "Config.cpp"
