namespace safini
{
namespace serialize
{

//converts passed string_view to the AnyTypeStorage that contains SerializedType
//for integer types only
template<typename SerializedType>
constexpr auto getSerizlizeFunc() requires std::is_integral_v<SerializedType>
{
    return [](const std::string_view str)->AnyTypeStorage
    {
        AnyTypeStorage toReturn(std::in_place_type_t<SerializedType>{}, std::stoll(std::string(str)));
        return toReturn;
    };
}

//general type conversion(tries SerializedType::SerializedType(str))
template<typename SerializedType>
constexpr auto getSerizlizeFunc() //seems that this one will always be chosen for substitution in the last
{
    return [](const std::string_view str)->AnyTypeStorage
    {
        AnyTypeStorage toReturn(std::in_place_type_t<SerializedType>{}, str);
        return toReturn;
    };
}

}
}
