#include "ConfigKeyRegister.cpp"
#include "MultipleException.hpp"

template<typename ConfigName>
safini::Config<ConfigName>::Config(const std::string_view filename):
    m_IniReader(filename)
{
    for(const auto& [key, typeHash, deserializeFunc, paramType] : _register::getRegisteredKeys<ConfigName>())
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
            m_KeysMap.emplace(std::make_pair(key, typeHash), deserializeFunc(param.value()));
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
template<typename ReturnType, const safini::StringLiteral key, const auto deserializeFunc>
const ReturnType& safini::Config<ConfigName>::extract() const noexcept
{
    //the code may break if you request volatile qualified thing
    //probably breaks at AnyTypeStorage.hpp:22
    //when you access volatile object through non volatile pointer, it's UB
    //see https://en.cppreference.com/w/cpp/language/cv
    //also see https://stackoverflow.com/questions/39583491/c-volatile-placement-new
    static_assert(!std::is_volatile_v<ReturnType>, "Volatile qualified parameters in config are not allowed");

    //registers the key to be a required key
    //(void) supresses warning -Wunused-value
    (void)_register::_registerKey<ConfigName,
                                  key,
                                  getHashFromType<const ReturnType>(),
                                  deserializeFunc,
                                  _register::Required>;

    return m_KeysMap.at(std::make_pair(std::string_view(key), getHashFromType<const ReturnType>())).template get<const ReturnType>();
}

template<typename ConfigName>
template<typename ReturnType, const safini::StringLiteral key, const auto deserializeFunc>
const ReturnType& safini::Config<ConfigName>::extractOr(const ReturnType& fallbackValue) const noexcept
{
    static_assert(!std::is_volatile_v<ReturnType>, "Volatile qualified parameters in config are not allowed");

    (void)_register::_registerKey<ConfigName,
                                  key,
                                  getHashFromType<const ReturnType>(),
                                  deserializeFunc,
                                  _register::Optional>;

    const auto param = m_KeysMap.find(std::make_pair(std::string_view(key), getHashFromType<const ReturnType>()));
    if(param == m_KeysMap.cend())
        return fallbackValue;
    return param->second.template get<const ReturnType>();
}

template<typename ConfigName>
template<typename ReturnType, const safini::StringLiteral key, const auto deserializeFunc>
std::optional<std::reference_wrapper<const ReturnType>> safini::Config<ConfigName>::tryExtract() const noexcept
{
    static_assert(!std::is_volatile_v<ReturnType>, "Volatile qualified parameters in config are not allowed");

    (void)_register::_registerKey<ConfigName,
                                  key,
                                  getHashFromType<const ReturnType>(),
                                  deserializeFunc,
                                  _register::Optional>;

    const auto param = m_KeysMap.find(std::make_pair(std::string_view(key), getHashFromType<const ReturnType>()));
    if(param == m_KeysMap.cend())
        return {};
    return param->second.template get<const ReturnType>();
}
