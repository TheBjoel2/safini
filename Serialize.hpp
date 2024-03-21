namespace safini
{
namespace serialize
{

//char type concept(see stdc++20 for more info)
template<typename CharType>
concept Character = std::same_as<CharType, char>
                 || std::same_as<CharType, unsigned char>;

//converts passed string_view to the AnyTypeStorage that contains SerializedType
//for char types only
template<typename SerializedType>
constexpr auto getSerizlizeFunc() requires Character<SerializedType>
{
    return [](const std::string_view str)->AnyTypeStorage
    {
        if(str.size() != 1) throw std::runtime_error("not a character");

        AnyTypeStorage toReturn(std::in_place_type_t<SerializedType>{}, str[0]);
        return toReturn;
    };
}

//for integer types only, excluding char types
template<typename SerializedType>
constexpr auto getSerizlizeFunc() requires std::is_integral_v<SerializedType>
                                        && (!Character<SerializedType>) //means not character concept
{
    return [](const std::string_view str)->AnyTypeStorage
    {
        AnyTypeStorage toReturn(std::in_place_type_t<SerializedType>{}, std::stoll(std::string(str)));
        return toReturn;
    };
}

//general type conversion(tries SerializedType::SerializedType(str))
template<typename SerializedType>
constexpr auto getSerizlizeFunc() requires std::convertible_to<const std::string_view, SerializedType>
{
    return [](const std::string_view str)->AnyTypeStorage
    {
        AnyTypeStorage toReturn(std::in_place_type_t<SerializedType>{}, str);
        return toReturn;
    };
}

}
}
