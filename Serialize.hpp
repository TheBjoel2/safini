#include <limits>

namespace safini
{
namespace serialize
{

//char type concept(see stdc++20 for more info)
template<typename CharType>
concept Character = std::same_as<CharType, char>
                 || std::same_as<CharType, unsigned char>;

//converts passed string_view to the AnyTypeStorage that contains DeserializedType
//for char types only
template<typename DeserializedType>
constexpr auto getDeserializeFunc() requires Character<DeserializedType>
{
    return [](const std::string_view str)->AnyTypeStorage
    {
        if(str.size() != 1) throw std::invalid_argument("convertible string is not a character");

        AnyTypeStorage toReturn(std::in_place_type_t<DeserializedType>{}, str[0]);
        return toReturn;
    };
}

//for signed integer types only, excluding char types
template<typename DeserializedType>
constexpr auto getDeserializeFunc() requires std::is_integral_v<DeserializedType>
                                          && std::is_signed_v<DeserializedType>
                                          && (!Character<DeserializedType>) //means not character concept
{
    return [](const std::string_view str)->AnyTypeStorage
    {
        const int64_t conversionResult = std::stoll(std::string(str));

        const bool flag1 = conversionResult >= std::numeric_limits<DeserializedType>::min();
        const bool flag2 = conversionResult <= std::numeric_limits<DeserializedType>::max();
        if(!flag1 || !flag2) throw std::out_of_range("convertible number out of range");

        AnyTypeStorage toReturn(std::in_place_type_t<DeserializedType>{}, static_cast<DeserializedType>(conversionResult));
        return toReturn;
    };
}

//for unsigned integer types only, excluding char types
template<typename DeserializedType>
constexpr auto getDeserializeFunc() requires std::is_integral_v<DeserializedType>
                                          && std::is_unsigned_v<DeserializedType>
                                          && (!Character<DeserializedType>) //means not character concept
{
    return [](const std::string_view str)->AnyTypeStorage
    {
        const uint64_t conversionResult = std::stoull(std::string(str));

        const bool flag1 = conversionResult >= std::numeric_limits<DeserializedType>::min();
        const bool flag2 = conversionResult <= std::numeric_limits<DeserializedType>::max();
        if(!flag1 || !flag2) throw std::out_of_range("convertible number out of range");

        AnyTypeStorage toReturn(std::in_place_type_t<DeserializedType>{}, static_cast<DeserializedType>(conversionResult));
        return toReturn;
    };
}

//general type conversion(tries DeserializedType::DeserializedType(str))
template<typename DeserializedType>
constexpr auto getDeserializeFunc() requires std::convertible_to<const std::string_view, DeserializedType>
{
    return [](const std::string_view str)->AnyTypeStorage
    {
        AnyTypeStorage toReturn(std::in_place_type_t<DeserializedType>{}, str);
        return toReturn;
    };
}

}
}
