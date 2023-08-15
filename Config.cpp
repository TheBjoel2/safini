#include "ConfigKeyRegister.cpp"
#include "Serialize.hpp"

namespace
{
    auto getParameter(const RxiIniReader& m_IniReader, const std::string_view key)
    {
        const std::size_t i = key.find('.');
        const bool hasSection = i != key.npos;
        //totally safe string operations
        const std::string name(hasSection ? std::next(std::begin(key), i+1) : std::begin(key), std::end(key));
        const std::string section(std::begin(key), hasSection ? std::next(std::begin(key), i) : std::begin(key));
        return m_IniReader.get(name, section);
    }
}

template<const StringLiteral configName>
safini::Config<configName>::Config(const std::string_view filename):
    m_IniReader(filename)
{
    for(const auto& [key, serializeFunc, destroyFunc, paramType] : _register::getRegisteredKeys<configName>())
    {
        const auto param = getParameter(m_IniReader, key);
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
const ReturnType& safini::Config<configName>::extract() noexcept
{
    //(void) supresses warning -Wunused-value
    (void)_register::_registerKey<configName,
                                  key,
                                  serialize::getSerizlizeFunc<ReturnType>(),
                                  serialize::getDestroyFunc<ReturnType>(),
                                  _register::Required>;

    return *std::launder(reinterpret_cast<ReturnType*>(m_KeysMap[std::string_view(key)].m_Data.data()));
}

template<const StringLiteral configName>
template<typename ReturnType, const StringLiteral key>
const ReturnType& safini::Config<configName>::extractOr(const ReturnType& fallbackValue) noexcept
{
    (void)_register::_registerKey<configName,
                                  key,
                                  serialize::getSerizlizeFunc<ReturnType>(),
                                  serialize::getDestroyFunc<ReturnType>(),
                                  _register::Optional>;

    const auto param = m_KeysMap.find(std::string_view(key));
    if(param == m_KeysMap.cend())
        return fallbackValue;
    return *std::launder(reinterpret_cast<ReturnType*>(param->second.m_Data.data()));
}

template<const StringLiteral configName>
template<typename ReturnType, const StringLiteral key>
std::optional<std::reference_wrapper<const ReturnType>> safini::Config<configName>::tryExtract() noexcept
{
    (void)_register::_registerKey<configName,
                                  key,
                                  serialize::getSerizlizeFunc<ReturnType>(),
                                  serialize::getDestroyFunc<ReturnType>(),
                                  _register::Optional>;

    const auto param = m_KeysMap.find(std::string_view(key));
    if(param == m_KeysMap.cend())
        return {};
    return *std::launder(reinterpret_cast<ReturnType*>(param->second.m_Data.data()));
}
