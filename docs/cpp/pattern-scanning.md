# Pattern Scanning

Soup is relatively popular for pattern scanning due to its high-speed engine that can scan at a rate of ~1 ms/pattern on a single thread (measured on an AMD 9950X3D).

The API for this is `Range::scan`. To get the range for the current process/module, you can use `Module(nullptr).range`. A `Pattern` instance can be constructed in various ways, but the `SIG_INST` macro, taking IDA-style patterns, is the preferred way for patterns known at compile-time.

```cpp
#include <soup/Module.hpp>
#include <soup/Pattern.hpp>
#include <soup/pattern_macros.hpp> // SIG_INST

SIG_INST("C3");
std::cout << Module(nullptr).range.scan(sig_inst).as<void*>(); // 00007FF_________
```
