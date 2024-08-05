#include "AnyTypeStorage.hpp"
#include <list>
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
    //the unique static storage(see below) will be created.
    //because imagine having 200 configs in your project, and they're all
    //stored in one single storage. Lame
    template<typename ConfigName>
    inline auto& _RegisteredKeysStorage()
    {
        //prevents some "static init order fiasco" by defining the static variable below inside a function
        //that's what stores all the parameters btw

        //oh yeah, you probably wonder why I store this data inside std::list instead of std::vector
        //well, if I store it inside vector, then occasionally, I will get a vector capacity upsize
        //that will invalidate every single reference to objects stored inside vector
        //we have quite a lot of such references, actually. look at what "inline const auto& _registerKey" returns
        static std::list<std::tuple<const KeyView,                                         //config parameter key
                                    std::function<AnyTypeStorage(const std::string_view)>, //deserialization function
                                    ParamType,                                             //whether the parameter is Required or Optional
                                    std::optional<AnyTypeStorage>>>                        //storage for a deserialized parameter
        toReturn;
        return toReturn;
    }

    //you declare this template in a function, it tries to instantiate _registerKey,
    //instantiation is possible only by executing lambda below,
    //lambda registers value. Simple.
    template<typename ConfigName,
             const StringLiteral registeredKey,
             auto deserializeFunc,
             ParamType paramType>
    inline const auto& _registerKey = std::invoke([]()->const std::optional<AnyTypeStorage>&
    {
        std::cout << std::string_view(registeredKey) << std::endl;
        _RegisteredKeysStorage<ConfigName>().emplace_back
        (
            registeredKey,
            deserializeFunc,
            paramType,
            std::nullopt
        );
        return std::get<3>(_RegisteredKeysStorage<ConfigName>().back()); //returns reference to std::optional from just created tuple in storage
    });
}

}
