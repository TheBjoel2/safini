#include "ini/src/ini.h"
#include <optional>

//do I really need to explain this part of a project?
namespace safini
{

class RxiIniReader
{
public:
    inline RxiIniReader(const std::string_view filename);
    //pointer member data. don't wanna deal with it
    RxiIniReader(const RxiIniReader&) = delete;
    RxiIniReader& operator=(const RxiIniReader&) = delete;

    inline ~RxiIniReader();

    //memory operations never throw
    inline std::optional<const std::string_view> get(const std::string_view name, const std::string_view section) const noexcept;

private:
    ini_t* m_Config;

    inline const char* read(const std::string_view name, const std::string_view section) const noexcept;
};

}

#include "RxiIniReader.cpp"
