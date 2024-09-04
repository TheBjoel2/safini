#ifndef TYPE_HASH
#define TYPE_HASH

#include <cstdint>

namespace safini
{

using TypeHash = const int32_t*;

template<typename>
struct TypeHashStruct
{
    static constexpr int32_t i{};
    static constexpr TypeHash value{&i};
};

template<typename T>
constexpr TypeHash getHashFromType()
{
    return TypeHashStruct<T>::value;
}

}

#endif // TYPE_HASH
