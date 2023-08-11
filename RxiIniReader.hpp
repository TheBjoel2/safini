#include "ini/src/ini.h"
#include <optional>

class RxiIniReader
{
public:
    RxiIniReader(const std::string_view filename);
    RxiIniReader(const RxiIniReader&) = delete;
    RxiIniReader& operator=(const RxiIniReader&) = delete;

    ~RxiIniReader();

    std::optional<const std::string_view> get(const std::string_view name, const std::string_view section) noexcept;

private:
    ini_t* m_Config;

    const char* read(const std::string_view name, const std::string_view section);
};

#include "RxiIniReader.cpp"
