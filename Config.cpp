#include "ConfigKeyRegister.cpp"
#include "Serialize.hpp"

template<const StringLiteral configName>
safini::Config<configName>::Config(const std::string_view filename):
    m_IniReader(filename)
{
    for(const auto& [key, serializeFunc, paramType] : _register::getRegisteredKeys<configName>())
    {
        //if map already contains that key, it's an error
        //because treating the same storage as different types is ub
        if(m_KeysMap.find(key) != m_KeysMap.end())
            throw std::runtime_error(std::string("Multiple keys \'")
                                         .append(key)
                                         .append("\' with different types are not allowed"));

        const std::size_t i = key.find('.');
        const bool hasSection = i != key.npos;
        //totally safe string operations
        const std::string name(hasSection ? std::next(std::begin(key), i+1) : std::begin(key), std::end(key));
        const std::string section(std::begin(key), hasSection ? std::next(std::begin(key), i) : std::begin(key));

        const auto param = m_IniReader.get(name, section);
        if(!param.has_value() && paramType == _register::Required)
            throw std::runtime_error(std::string("No parameter \'")
                                         .append(key)
                                         .append("\' in config file \'")
                                         .append(filename)
                                         .append(1, '\''));
        if(!param.has_value() && paramType == _register::Optional)
            continue;
        try
        {
            m_KeysMap.emplace(key, serializeFunc(param.value()));
        }
        catch(...)
        {
            if(paramType == _register::Required)
                throw std::runtime_error(std::string("Unable to convert \'")
                                             .append(param.value())
                                             .append("\' to whatever type it belongs to")
                                             .append(" from config file \'")
                                             .append(filename)
                                             .append(1, '\''));
        }
    }
}

template<const StringLiteral configName>
template<typename ReturnType, const StringLiteral key>
const ReturnType& safini::Config<configName>::extract() const noexcept
{
    //registers the key to be a required key
    //(void) supresses warning -Wunused-value
    (void)_register::_registerKey<configName,
                                  key,
                                  serialize::getSerizlizeFunc<ReturnType>(),
                                  _register::Required>;

    return m_KeysMap.at(std::string_view(key)).get<ReturnType>();
}

template<const StringLiteral configName>
template<typename ReturnType, const StringLiteral key>
const ReturnType& safini::Config<configName>::extractOr(const ReturnType& fallbackValue) const noexcept
{
    (void)_register::_registerKey<configName,
                                  key,
                                  serialize::getSerizlizeFunc<ReturnType>(),
                                  _register::Optional>;

    const auto param = m_KeysMap.find(std::string_view(key));
    if(param == m_KeysMap.cend())
        return fallbackValue;
    return param->second.get<ReturnType>();
}

template<const StringLiteral configName>
template<typename ReturnType, const StringLiteral key>
std::optional<std::reference_wrapper<const ReturnType>> safini::Config<configName>::tryExtract() const noexcept
{
    (void)_register::_registerKey<configName,
                                  key,
                                  serialize::getSerizlizeFunc<ReturnType>(),
                                  _register::Optional>;

    const auto param = m_KeysMap.find(std::string_view(key));
    if(param == m_KeysMap.cend())
        return {};
    return param->second.get<ReturnType>();
}
