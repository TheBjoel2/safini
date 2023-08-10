#include <vector>
#include <functional>

//if you try accessing this, your computer may set on fire.
//UPD: lol it also set on fire my microwave
namespace _register
{
    using NameView    = std::string_view;
    using SectionView = std::string_view;

    template<const StringLiteral configName>
    auto& _RegisteredKeysVector()
    {
        //prevents some "static init order fiasco" because it's inside a function
        static std::vector<std::pair<const SectionView, const NameView>> toReturn;
        return toReturn;
    }

    template<const StringLiteral configName,
             const StringLiteral registeredName,
             const StringLiteral registeredSection>
    static const auto _registerKey = std::invoke([]
    {
        _RegisteredKeysVector<configName>().emplace_back
        (
            registeredSection,
            registeredName
        );
        return 0;
    });
}

//you can call that
template<const StringLiteral configName>
const auto& getRegisteredKeys()
{
    return _register::_RegisteredKeysVector<configName>();
}
