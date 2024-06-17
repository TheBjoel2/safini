#include <limits>

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
        if(str.size() != 1) throw std::runtime_error("convertible string is not a character");

        AnyTypeStorage toReturn(std::in_place_type_t<SerializedType>{}, str[0]);
        return toReturn;
    };
}

//for signed integer types only, excluding char types
template<typename SerializedType>
constexpr auto getSerizlizeFunc() requires std::is_integral_v<SerializedType>
                                        && std::is_signed_v<SerializedType>
                                        && (!Character<SerializedType>) //means not character concept
{
    return [](const std::string_view str)->AnyTypeStorage
    {
        const int64_t conversionResult = std::stoll(std::string(str));

        const bool flag1 = conversionResult >= std::numeric_limits<SerializedType>::min();
        const bool flag2 = conversionResult <= std::numeric_limits<SerializedType>::max();
        if(!flag1 || !flag2) throw std::out_of_range("convertible number out of range");

        AnyTypeStorage toReturn(std::in_place_type_t<SerializedType>{}, static_cast<SerializedType>(conversionResult));
        return toReturn;
    };
}

//for unsigned integer types only, excluding char types
template<typename SerializedType>
constexpr auto getSerizlizeFunc() requires std::is_integral_v<SerializedType>
                                        && std::is_unsigned_v<SerializedType>
                                        && (!Character<SerializedType>) //means not character concept
{
    return [](const std::string_view str)->AnyTypeStorage
    {
        const uint64_t conversionResult = std::stoull(std::string(str));

        const bool flag1 = conversionResult >= std::numeric_limits<SerializedType>::min();
        const bool flag2 = conversionResult <= std::numeric_limits<SerializedType>::max();
        if(!flag1 || !flag2) throw std::out_of_range("convertible number out of range");

        AnyTypeStorage toReturn(std::in_place_type_t<SerializedType>{}, static_cast<SerializedType>(conversionResult));
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
