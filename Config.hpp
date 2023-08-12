#include "StringLiteral.hpp"
#include "RxiIniReader.hpp"
#include <string_view>
#include <optional>
#include <unordered_map>
#include <vector>
#include <functional>

namespace safini
{

template<const StringLiteral configName>
class Config
{
public:
    Config(const std::string_view filename);

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
    using Storage = std::vector<char>;

    struct SelfDestroyingStorage
    {
        Storage m_Data;
        std::function<void(std::vector<char>&)> destroyFunc;

        ~SelfDestroyingStorage();
    };

    std::unordered_map<FullKey, SelfDestroyingStorage> m_KeysMap{};
};

}

#include "Config.cpp"
