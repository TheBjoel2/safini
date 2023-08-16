#include "ConfigKeyRegister.cpp"
#include "Serialize.hpp"

template<const StringLiteral configName>
safini::Config<configName>::Config(const std::string_view filename):
    m_IniReader(filename)
{
    for(const auto& [key, serializeFunc, destroyFunc, paramType] : _register::getRegisteredKeys<configName>())
    {
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
            //brainfuck style emplacing element just not to call ~SelfDestroyingStorage() here(it may break things)
            //ngl address sanitizer said nothing about that, but I'm paranoid
            m_KeysMap.emplace(std::piecewise_construct,
                              std::forward_as_tuple(key),
                              std::forward_as_tuple(destroyFunc, serializeFunc(param.value())));
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
safini::Config<configName>::SelfDestroyingStorage::~SelfDestroyingStorage()
{
    destroyFunc(m_Data);
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
                                  serialize::getDestroyFunc<ReturnType>(),
                                  _register::Required>;

    //memory laundering because we store objects in vectors of chars
    return *std::launder(reinterpret_cast<const ReturnType*>(m_KeysMap.at(std::string_view(key)).m_Data.data()));
}

template<const StringLiteral configName>
template<typename ReturnType, const StringLiteral key>
const ReturnType& safini::Config<configName>::extractOr(const ReturnType& fallbackValue) const noexcept
{
    (void)_register::_registerKey<configName,
                                  key,
                                  serialize::getSerizlizeFunc<ReturnType>(),
                                  serialize::getDestroyFunc<ReturnType>(),
                                  _register::Optional>;

    const auto param = m_KeysMap.find(std::string_view(key));
    if(param == m_KeysMap.cend())
        return fallbackValue;
    return *std::launder(reinterpret_cast<const ReturnType*>(param->second.m_Data.data()));
}

template<const StringLiteral configName>
template<typename ReturnType, const StringLiteral key>
std::optional<std::reference_wrapper<const ReturnType>> safini::Config<configName>::tryExtract() const noexcept
{
    (void)_register::_registerKey<configName,
                                  key,
                                  serialize::getSerizlizeFunc<ReturnType>(),
                                  serialize::getDestroyFunc<ReturnType>(),
                                  _register::Optional>;

    const auto param = m_KeysMap.find(std::string_view(key));
    if(param == m_KeysMap.cend())
        return {};
    return *std::launder(reinterpret_cast<const ReturnType*>(param->second.m_Data.data()));
}
