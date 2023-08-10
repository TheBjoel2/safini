#include "ConfigKeyRegister.cpp"
#include "Cast.hpp"

template<const StringLiteral filename>
safini::Config<filename>::Config():
    m_IniReader(std::string_view(filename.value, filename.size))
{
    for(const auto& [section, name] : getRegisteredKeys<filename>())
    {
        const std::string key = std::string(section).append(1, '.').append(name);

        const auto param = m_IniReader.get(name, section);
        if(!param.has_value())
        {
            const std::string str = std::string("No parameter ")
                                        .append(section)
                                        .append(1, '.')
                                        .append(name)
                                        .append(" in config file ")
                                        .append(filename);
            throw std::runtime_error(str);
        }
        m_KeysMap[key] = param.value();
    }
}

template<const StringLiteral filename>
template<typename ReturnType, const StringLiteral name, const StringLiteral section>
ReturnType safini::Config<filename>::extract()
{
    (void)_register::_registerKey<filename, name, section>; //(void) supresses warning -Wunused-value

    const std::string key = std::string(std::string_view(section)).append(1, '.').append(std::string_view(name));
    return cast::selectiveCast<ReturnType>(m_KeysMap[key]);
}
