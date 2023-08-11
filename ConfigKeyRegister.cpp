#include <vector>
#include <functional>

namespace safini
{

//if you try accessing this, your computer may set on fire.
//UPD: lol it also set on fire my microwave
namespace _register
{
    using NameView    = std::string_view;
    using SectionView = std::string_view;

    template<const StringLiteral configName>
    auto& _RegisteredKeysVector()
    {
        //prevents some "static init order fiasco" by defining the static variable below inside a function
        static std::vector<std::tuple<const SectionView,
                                      const NameView,
                                      std::function<std::vector<char>(const std::string_view)>,
                                      std::function<void(std::vector<char>&)>>> toReturn;
        return toReturn;
    }

    template<const StringLiteral configName,
             const StringLiteral registeredName,
             const StringLiteral registeredSection,
             auto serializeFunc,
             auto destroyFunc>
    static const auto _registerKey = std::invoke([]
    {
        _RegisteredKeysVector<configName>().emplace_back
        (
            registeredSection,
            registeredName,
            serializeFunc,
            destroyFunc
        );
        return 0;
    });

    //you can call that
    template<const StringLiteral configName>
    const auto& getRegisteredKeys() noexcept
    {
        return _register::_RegisteredKeysVector<configName>();
    }
}

}
