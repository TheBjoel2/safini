#include "StringLiteral.hpp"
#include "RxiIniReader.hpp"
#include <string_view>
#include <optional>
#include <unordered_map>

namespace safini
{

template<const StringLiteral filename>
class Config
{
public:
    Config();

    template<typename ReturnType,
             const StringLiteral name,
             const StringLiteral section = "">
    auto extract()
    -> ReturnType;

    template<typename ReturnType,
             const StringLiteral name,
             const StringLiteral section = "">
    auto extractOr(const ReturnType& fallbackValue)
    -> ReturnType;

    template<typename ReturnType,
             const StringLiteral name,
             const StringLiteral section = "">
    auto tryExtract()
    -> std::optional<ReturnType>;

private:
    RxiIniReader m_IniReader;

    using FullKey = std::string;
    using Value = std::string;
    std::unordered_map<FullKey, Value> m_KeysMap{};
};

}

#include "Config.cpp"
