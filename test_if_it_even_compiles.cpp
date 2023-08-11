#include <iostream>
#include "Config.hpp"

class Numbrr
{
public:
    Numbrr(const std::string_view n)
    {
        std::cout << "N: " << n << std::endl;
    }

    ~Numbrr()
    {
        std::cout << "~N()" << std::endl;
    }
};

int32_t main()
{
    safini::Config<"config1"> c("what.ini");
    std::cout << c.extract<int, "num">() << std::endl;
    std::cout << c.extract<const std::string_view, "eh", "lol">() << std::endl;

    c.extract<Numbrr, "ok">();
    return 0;
}
