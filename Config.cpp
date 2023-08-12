#include "ConfigKeyRegister.cpp"
#include "Serialize.hpp"

template<const StringLiteral configName>
safini::Config<configName>::Config(const std::string_view filename):
    m_IniReader(filename)
{
    for(const auto& [section, name, serializeFunc, destroyFunc] : _register::getRegisteredKeys<configName>())
    {
        const std::string key = std::string(section).append(1, '.').append(name);

        const auto param = m_IniReader.get(name, section);
        if(!param.has_value())
            throw std::runtime_error(std::string("No parameter \'")
                                         .append(section)
                                         .append(1, '.')
                                         .append(name)
                                         .append("\' in config file \'")
                                         .append(filename)
                                         .append(1, '\''));
        try
        {
            //brainfuck style emplacing element just not to call ~SelfDestroyingStorage() here(it may break things)
            //ngl address sanitizer said nothing about that, but I'm paranoid
            m_KeysMap.emplace(std::piecewise_construct,
                              std::forward_as_tuple(key),
                              std::forward_as_tuple(serializeFunc(param.value()), destroyFunc));
        }
        catch(...)
        {
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
template<typename ReturnType, const StringLiteral name, const StringLiteral section>
const ReturnType& safini::Config<configName>::extract() noexcept
{
    //(void) supresses warning -Wunused-value
    (void)_register::_registerKey<configName,
                                  name,
                                  section,
                                  serialize::getSerizlizeFunc<ReturnType>(),
                                  serialize::getDestroyFunc<ReturnType>()>;

    const std::string key = std::string(section).append(1, '.').append(name);
    return *std::launder(reinterpret_cast<ReturnType*>(m_KeysMap[key].m_Data.data()));
}

template<const StringLiteral configName>
safini::Config<configName>::SelfDestroyingStorage::~SelfDestroyingStorage()
{
    destroyFunc(m_Data);
}
