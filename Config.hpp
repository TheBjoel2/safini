#include "StringLiteral.hpp"
#include "RxiIniReader.hpp"
#include <string_view>
#include <optional>
#include <unordered_map>
#include <vector>

namespace safini
{

template<const StringLiteral configName>
class Config
{
public:
    Config(const std::string_view filename);
    ~Config();

    template<typename ReturnType,
             const StringLiteral name,
             const StringLiteral section = "">
    auto extract() noexcept
    -> const ReturnType&;

    template<typename ReturnType,
             const StringLiteral name,
             const StringLiteral section = "">
    auto extractOr(const ReturnType& fallbackValue) noexcept
    -> const ReturnType&;

    template<typename ReturnType,
             const StringLiteral name,
             const StringLiteral section = "">
    auto tryExtract() noexcept
    -> std::optional<const ReturnType&>;

private:
    RxiIniReader m_IniReader;

    using FullKey = std::string;
    using Value = std::vector<char>;
    std::unordered_map<FullKey, Value> m_KeysMap{};
};

}

#include "Config.cpp"
