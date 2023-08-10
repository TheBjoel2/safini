#include <cstdint>
#include <algorithm>

inline void expectedNullTerminatedArray(){}

template<std::size_t _size>
struct StringLiteral
{
    consteval StringLiteral(const char (&str)[_size]):
        size(_size)
    {
        if(str[_size-1] != '\0')
            expectedNullTerminatedArray();

        std::copy_n(str, _size, value);
    }

    operator std::string_view()
    {
        return {value, size-1};
    }

    operator const std::string_view() const
    {
        return {value, size-1};
    }

    char value[_size];
    std::size_t size;
};
