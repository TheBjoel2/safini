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
    try
    {
        safini::Config<"config1"> c("what.ini");
        std::cout << c.extract<int, "num">() << std::endl;
        std::cout << c.extract<const std::string, "lol.eh">() << std::endl;
        std::cout << c.extract<const std::string_view, "fail">() << std::endl;
        c.extract<Numbrr, "ok">();
        std::cout << c.extractOr<uint32_t, "uwu">(420) << std::endl;
        std::cout << c.tryExtract<char, "hehehe">().has_value() << std::endl;
    }
    catch(const std::runtime_error& err)
    {
        std::cout << err.what() << std::endl;
    }
    return 0;
}
