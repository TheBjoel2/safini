# safini
C++20 header-only .ini configuration file reader library.
Easy to set up, easy to use. Should work on all platforms.

# Setup
Go to your project's root and do:
```
git clone --recursive https://github.com/TheBjoel2/safini.git
```
Include this
```cpp
#include "safini/Config.hpp"
```

# Usage
Using safini is as simple as
```cpp
//what.ini - filename in a filesystem
//config1 - just a config identifier. Set this to unique value for every safini::Config instance
safini::Config<class config1> cfg("what.ini");

cfg.extract   <int,                    "num">();
cfg.extractOr <const std::string,      "foo.bar">("Nyah");
cfg.tryExtract<const std::string_view, "jopa">(); //returns std::optional
```
btw when calling extract() with the key not present in the config file,
or the value is not convertible to the requested type, it will throw at ``Config::Config()``.
If you don't understand what I just said, just look at the backtrace of such crash
```
(gdb) bt
...
#6  std::terminate () at /usr/src/debug/gcc/gcc/libstdc++-v3/libsupc++/eh_terminate.cc:58
#7  __cxxabiv1::__cxa_throw (obj=<optimized out>, tinfo=0x55555556ad00 <typeinfo for std::runtime_error@GLIBCXX_3.4>, dest=0x7ffff7cc8320 <std::runtime_error::~runtime_error()>)
#8  safini::Config::Config(...) at Config.cpp:18 <------ Look here
...
```
This happens because when you call ``extract()``, ``extractOr()`` or ``tryExtract()``,
it places keys on a static vector using some template magic that I don't fully understand,
but people of stackoverflow suggested this code.
``extractOr()`` and ``tryExtract()`` don't cause the crash.

Note that every object you read from config is stored in the internal storage of Config instance.
That's why we return const references to them.
And that's why **their lifetime is the same as the Config instance's lifetime.**

The value presented in a config file could be converted to string with no problem,
but for example conversion to ``int`` requires the value to be an actual number.
One day you may also need to convert it to ``SomeWeirdComplexTypeAbstractFactorySingletonThreadLocal``,
which would require additional conversion logic.
You could define such additional logic either by creating a constructor of ``SomeWeirdComplexTypeAbstractFactorySingletonThreadLocal`` that takes ``const std::string_view`` as a parameter,
or by calling ``extract``, ``extractOr`` or ``tryExtract`` as follows:
```cpp
cfg.extract<SomeWeirdComplexTypeAbstractFactorySingletonThreadLocal,
            "num",
            [](const std::string_view str)->AnyTypeStorage
{
    //here you construct and return AnyTypeStorage containing your type
    //you can find examples in Serialize.hpp
    //returning AnyTypeStorage containing not the requested type is UB
}>();
```
If this lambda throws, this means that conversion has been failed.

Use wisely. Report bugs.

# But why?
So, I had a small project and I needed to quickly add some kind of a config to it.
Tried 4 repos.
The first one was ok, but overabstracted. And also json.
The second one was also ok... But the documentation was in chinese, so I couldn't read it.
The third one crashed with SIGSEGV when I tried to use it.
The fourth one was implemented in C, so I wrote an abstraction over it for my needs. Hope someone will find this useful.
