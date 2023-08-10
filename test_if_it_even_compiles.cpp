#include <iostream>
#include "Config.hpp"

int32_t main()
{
    safini::Config<"what.ini"> c;
    std::cout << c.extract<int, "num">() << std::endl;
    std::cout << c.extract<const std::string_view, "eh", "lol">() << std::endl;
    c.extract<const std::optional<int>, "ok">();
    return 0;
}
