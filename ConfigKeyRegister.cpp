#include "AnyTypeStorage.hpp"
#include <vector>
#include <functional>

namespace safini
{

//if you try accessing this namespace, your computer may set on fire.
//UPD: lol it also set on fire my microwave
namespace _register
{
    using KeyView = std::string_view;

    enum ParamType
    {
        Required,
        Optional
    };

    //this's a template function because for every function template,
    //the unique static vector(see below) will be created.
    //because imagine having 200 configs in your project, and they're all
    //stored in one single vector. Lame
    template<const StringLiteral configName>
    inline auto& _RegisteredKeysVector()
    {
        //prevents some "static init order fiasco" by defining the static variable below inside a function
        //that's what stores all the parameters btw
        static std::vector<std::tuple<const KeyView,
                                      std::function<AnyTypeStorage(const std::string_view)>,
                                      ParamType>> toReturn;
        return toReturn;
    }

    //you declare this template in a function, it tries to instantiate _registerKey,
    //instantiation is possible only by executing lambda below,
    //lambda registers value. Simple.
    template<const StringLiteral configName,
             const StringLiteral registeredKey,
             auto serializeFunc,
             ParamType paramType>
    inline const auto _registerKey = std::invoke([]
    {
        _RegisteredKeysVector<configName>().emplace_back
        (
            registeredKey,
            serializeFunc,
            paramType
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
