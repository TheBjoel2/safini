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
        std::construct_at(reinterpret_cast<SerializedType*>(toReturn.data()), std::stoll(std::string(str)));
        return toReturn;
    };
}

template<typename SerializedType>
constexpr auto getSerizlizeFunc() //seems that this one will always be chosen for substitution in the last
{
    return [](const std::string_view str)->std::vector<char>
    {
        std::vector<char> toReturn(sizeof(SerializedType));
        std::construct_at(reinterpret_cast<SerializedType*>(toReturn.data()), str);
        return toReturn;
    };
}

//generally works for all types ig
template<typename DestroyedType>
constexpr auto getDestroyFunc()
{
    return [](std::vector<char>& vec)->void
    {
        std::destroy_at(std::launder(reinterpret_cast<DestroyedType*>(vec.data())));
    };
}

}
}
