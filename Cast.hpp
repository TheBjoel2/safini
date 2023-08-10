namespace safini
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
}
