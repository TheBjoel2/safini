#include "ConfigKeyRegister.cpp"
#include "MultipleException.hpp"

template<typename ConfigName>
safini::Config<ConfigName>::Config(const std::string_view filename):
    m_IniReader(filename)
{
    for(auto& entry : _register::_RegisteredKeysStorage<ConfigName>())
    {
        //oh, I would love to use structured binding instead of that crap,
        //however you can't mix auto& and const auto& together
        const auto& key           = std::get<0>(entry);
        const auto& serializeFunc = std::get<1>(entry);
        const auto& paramType     = std::get<2>(entry);
        std::optional<AnyTypeStorage>& optionalStorage     = std::get<3>(entry);

        //splitting your "section.key" into "section" and "section keys"
        //see https://en.wikipedia.org/wiki/INI_file
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
            optionalStorage.emplace(serializeFunc(param.value()));
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
safini::Config<ConfigName>::~Config()
{
    //need to clear that storage like so because I'm guaranteeing to destruct
    //objects stored in storage in the reverse order of them constructed
    auto& keysStorage = _register::_RegisteredKeysStorage<ConfigName>();
    while(!keysStorage.empty())
        keysStorage.pop_back();
}

template<typename ConfigName>
template<typename ReturnType, const safini::StringLiteral key, auto serializeFunc>
const ReturnType& safini::Config<ConfigName>::extract() const noexcept
{
    //the code may break if you request volatile qualified thing
    //probably breaks at AnyTypeStorage.hpp:22
    //when you access volatile object through non volatile pointer, it's UB
    //see https://en.cppreference.com/w/cpp/language/cv
    //also see https://stackoverflow.com/questions/39583491/c-volatile-placement-new
    static_assert(!std::is_volatile_v<ReturnType>, "Volatile qualified parameters in config are not allowed");

    //registers the key to be a required key
    const auto& optionalRef = _register::_registerKey<ConfigName,
                                                      key,
                                                      serializeFunc,
                                                      _register::Required>;
    //optional is guaranteed to have a value
    return optionalRef.value().template get<const ReturnType>();
}

template<typename ConfigName>
template<typename ReturnType, const safini::StringLiteral key, auto serializeFunc>
const ReturnType& safini::Config<ConfigName>::extractOr(const ReturnType& fallbackValue) const noexcept
{
    static_assert(!std::is_volatile_v<ReturnType>, "Volatile qualified parameters in config are not allowed");

    const auto& optionalRef = _register::_registerKey<ConfigName,
                                                      key,
                                                      serializeFunc,
                                                      _register::Required>;
    if(!optionalRef.has_value())
        return fallbackValue;
    return optionalRef.value().template get<const ReturnType>();
}

template<typename ConfigName>
template<typename ReturnType, const safini::StringLiteral key, auto serializeFunc>
std::optional<std::reference_wrapper<const ReturnType>> safini::Config<ConfigName>::tryExtract() const noexcept
{
    static_assert(!std::is_volatile_v<ReturnType>, "Volatile qualified parameters in config are not allowed");

    const auto& optionalRef = _register::_registerKey<ConfigName,
                                                      key,
                                                      serializeFunc,
                                                      _register::Required>;
    if(!optionalRef.has_value())
        return {};
    return optionalRef.value().template get<const ReturnType>();
}
