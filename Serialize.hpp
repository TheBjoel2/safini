/*namespace safini
{
namespace cast
{

template<typename ConvertTo> requires std::is_arithmetic_v<ConvertTo>
ConvertTo convert(const auto& converted)
{
    return std::stoll(converted);
}

template<typename CastTo>
CastTo selectiveCast(const auto& casted) requires std::convertible_to<decltype(casted), CastTo>
{
    return static_cast<CastTo>(casted);
}

template<typename CastTo>
CastTo selectiveCast(const auto& casted)
{
    return convert<CastTo>(casted);
}

}
}*/

namespace safini
{
namespace serialize
{

template<typename SerializedType>
constexpr auto getSerizlizeFunc() requires std::is_arithmetic_v<SerializedType>
{
    return [](const std::string_view str)->std::vector<char>
    {
        std::vector<char> toReturn(sizeof(SerializedType));
        new (toReturn.data()) SerializedType(std::stoll(std::string(str)));
        return toReturn;
    };
}
template<typename DestroyedType>
constexpr auto getDestroyFunc() requires std::is_arithmetic_v<DestroyedType>
{
    return [](std::vector<char>& vec)->void
    {
        std::destroy_at(std::launder(reinterpret_cast<DestroyedType*>(vec.data())));
    };
}

template<typename SerializedType>
constexpr auto getSerizlizeFunc() requires std::convertible_to<const std::string_view, SerializedType>
{
    return [](const std::string_view str)->std::vector<char>
    {
        std::vector<char> toReturn(sizeof(SerializedType));
        new (toReturn.data()) SerializedType(str);
        return toReturn;
    };
}
template<typename DestroyedType>
constexpr auto getDestroyFunc() requires std::convertible_to<const std::string_view, DestroyedType>
{
    return [](std::vector<char>& vec)->void
    {
        std::destroy_at(std::launder(reinterpret_cast<DestroyedType*>(vec.data())));
    };
}

}
}
