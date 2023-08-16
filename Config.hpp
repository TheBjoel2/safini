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
             const StringLiteral key>
    auto extract() const noexcept
    -> const ReturnType&;

    template<typename ReturnType,
             const StringLiteral key>
    auto extractOr(const ReturnType& fallbackValue) const noexcept
    -> const ReturnType&;

    template<typename ReturnType,
             const StringLiteral key>
    auto tryExtract() const noexcept
    -> std::optional<std::reference_wrapper<const ReturnType>>;

private:
    RxiIniReader m_IniReader;

    using Key = std::string_view;
    using Storage = std::vector<char>;

    struct SelfDestroyingStorage
    {
        std::function<void(std::vector<char>&)> destroyFunc;
        Storage m_Data;

        ~SelfDestroyingStorage();
    };

    std::unordered_map<Key, SelfDestroyingStorage> m_KeysMap{};
};

}

#include "Config.cpp"
