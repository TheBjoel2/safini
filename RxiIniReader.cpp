RxiIniReader::RxiIniReader(const std::string_view filename):
    m_Config(ini_load(filename.data()))
{
    if(!m_Config)
        throw std::runtime_error(std::string("Unable to load config file ").append(filename));
}

RxiIniReader::~RxiIniReader()
{
    ini_free(m_Config);
}

const char* RxiIniReader::read(const std::string_view name, const std::string_view section)
{
    if(section.empty())
        return ini_get(m_Config, nullptr, name.data());
    return ini_get(m_Config, section.data(), name.data());
}

std::optional<const std::string_view> RxiIniReader::get(const std::string_view name, const std::string_view section) noexcept
{
    const char* param = read(name, section);
    if(!param)
        return {};
    return param;
}
