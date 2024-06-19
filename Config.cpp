#include "ConfigKeyRegister.cpp"
#include "MultipleException.hpp"

template<typename ConfigName>
safini::Config<ConfigName>::Config(const std::string_view filename):
    m_IniReader(filename)
{
    for(const auto& [key, typeHash, serializeFunc, paramType] : _register::getRegisteredKeys<ConfigName>())
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
            m_KeysMap.emplace(std::make_pair(key, typeHash), serializeFunc(param.value()));
        }
        catch(const std::exception& exc)
        {
            if(paramType == _register::Required)
            {
                MultipleException multipleException(exc.what());
                multipleException.addException(std::string("Unable to convert \'")
                                                   .append(key)
                                                   .append("\'=\'")
                                                   .append(param.value())
                                                   .append("\' to whatever type it belongs to")
                                                   .append(" from config file \'")
                                                   .append(filename)
                                                   .append(1, '\''));
                throw multipleException;
            }
        }
    }
}

template<typename ConfigName>
template<typename ReturnType, const safini::StringLiteral key, auto serializeFunc>
const ReturnType& safini::Config<ConfigName>::extract() const noexcept
{
    //registers the key to be a required key
    //(void) supresses warning -Wunused-value
    (void)_register::_registerKey<ConfigName,
                                  key,
                                  getHashFromType<ReturnType>(),
                                  serializeFunc,
                                  _register::Required>;

    return m_KeysMap.at(std::make_pair(std::string_view(key), getHashFromType<ReturnType>())).template get<const ReturnType>();
}

template<typename ConfigName>
template<typename ReturnType, const safini::StringLiteral key, auto serializeFunc>
const ReturnType& safini::Config<ConfigName>::extractOr(const ReturnType& fallbackValue) const noexcept
{
    (void)_register::_registerKey<ConfigName,
                                  key,
                                  getHashFromType<ReturnType>(),
                                  serializeFunc,
                                  _register::Optional>;

    const auto param = m_KeysMap.find(std::make_pair(std::string_view(key), getHashFromType<ReturnType>()));
    if(param == m_KeysMap.cend())
        return fallbackValue;
    return param->second.template get<const ReturnType>();
}

template<typename ConfigName>
template<typename ReturnType, const safini::StringLiteral key, auto serializeFunc>
std::optional<std::reference_wrapper<const ReturnType>> safini::Config<ConfigName>::tryExtract() const noexcept
{
    (void)_register::_registerKey<ConfigName,
                                  key,
                                  getHashFromType<ReturnType>(),
                                  serializeFunc,
                                  _register::Optional>;

    const auto param = m_KeysMap.find(std::make_pair(std::string_view(key), getHashFromType<ReturnType>()));
    if(param == m_KeysMap.cend())
        return {};
    return param->second.template get<const ReturnType>();
}
