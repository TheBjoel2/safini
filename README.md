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
#8  safini::Config::Config(...) at Config.cpp:17 <------ Look here
...
```
This happens because when you instantiate ``extract()``, ``extractOr()`` or ``tryExtract()``,
it places keys on a static vector before the start of a `int main()` function,
thanks to some template variable instantiation magic
that I don't fully understand, but people of stackoverflow suggested this code.
And then, when you call ``Config::Config()``, it loads all the parameters from the config file,
converts them to the requested types and places them into ``std::unordered_map``. And, if something goes wrong
during the process(e.g. the parameter is not present in the file/can't be converted to the requested type),
it throws an exception at ``Config::Config()``. More on the exceptions later in the document.
``extractOr()`` and ``tryExtract()`` don't cause the crash at ``Config::Config()`` in these cases, but they are also
loaded during the construction of ``Config`` instance.

Note that every object you read from the config is stored in the internal storage of ``Config`` instance.
That's why we return const references to the stored objects.
And that's why **their lifetime is the same as the ``Config`` instance's lifetime.**

Now to the exceptions.
- ``extract()``, ``extractOr()`` and ``tryExtract()`` are ``noexcept``, meaning that
they do not throw any exceptions, and won't ever throw them in the future (you can rely on that).
- ``Config::Config()`` does throw several exceptions, and **all of them could be caught by ``std::exception`` reference**
  - ``std::runtime_error`` if ``RxiIniReader`` fails to load the config file into memory (e.g. file does not exist)
  - whatever is thrown by ``std::string`` constructor and its ``append`` method (e.g. ``std::bad_alloc``)
  - ``std::runtime_error`` if the key you parse with ``extract()`` method is not present in the config file
  - ``MultipleException`` if the emplacement of the parameter into ``std::unordered_map`` fails.
    ``MultipleException`` will contain its own error message as well as the error message of the exception
    that caused the emplacement failure. It could contain exception message of the exception thrown by
    - ``std::unordered_map::emplace()``, whatever it throws (e.g. ``std::bad_alloc``)
    - ``safini::serialize`` functions, whatever they throw (see ``Serialize.hpp``)
    - your own serialization function/lambda (see below), whatever it throws
  - whatever is thrown by ``std::runtime_error`` constructor
  - whatever is thrown by ``MultipleException`` constructor
  - If you pass a string that is not null terminated to a ``Config`` constructor as a filename parameter,
    the behavior is undefined

``MultipleException`` is a class inherited from ``std::exception`` that has similar interface to all exception classes
(e.g. ``noexcept`` copy constructor, ``noexcept what()``) with the addition of ``addException()`` method to
aggregate exception messages, and all of them can be obtained with the ``what()`` method.

The value presented in a config file could be converted to string with no problem,
but for example conversion to ``int`` requires the value to be an actual number.
Additional logic defined in ``Serialize.hpp`` will be used to convert it from string to number.
One day you may also need to convert it to ``SomeWeirdComplexTypeAbstractFactorySingletonThreadLocal``,
for which we do not have a conversion function. And it will fail to compile.
To fix this, you could define such additional logic either by creating a constructor of
``SomeWeirdComplexTypeAbstractFactorySingletonThreadLocal`` that takes ``const std::string_view``
as a parameter, or by calling ``extract``, ``extractOr`` or ``tryExtract`` as follows:
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
If this lambda throws, this means that conversion has been failed. The corresponding exception (see above)
will be thrown by ``Config::Config()`` in that case;

Use wisely. Report bugs. Feature requests are welcome.

# But why?
So, I had a small project and I needed to quickly add some kind of a config to it.
Tried 4 repos.
The first one was ok, but overabstracted. And also json.
The second one was also ok... But the documentation was in chinese, so I couldn't read it.
The third one crashed with SIGSEGV when I tried to use it.
The fourth one was implemented in C, so I wrote an abstraction over it for my needs. Hope someone will find this useful.
