#include <cstdint>
#include <algorithm>
#include <string_view>

namespace safini
{

inline void expectedNullTerminatedArray(){}

//yeah it's c++20, yet no other way to pass "" as template parameter
template<std::size_t _size>
struct StringLiteral
{
    consteval StringLiteral(const char (&str)[_size]):
        size(_size)
    {
        //this does nothing but causes compile time error if the thing is not null terminated
        if(str[_size-1] != '\0')
            expectedNullTerminatedArray();

        std::copy_n(str, _size, value);
    }

    //is this formatting good? I need your opinion
    //because hole                    hole
    //         |                        |
    //         v                        v
    operator       std::string_view()       { return {value, size-1}; }
    operator const std::string_view() const { return {value, size-1}; }

    char value[_size];
    std::size_t size;
};

}
