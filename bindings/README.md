# Soup Bindings

> [!NOTE]
> Soup is primarily a C++ library and the amount of functions that have a C ABI wrapper is an embarrassingly small subset.
> Nevertheless, a not insignificant amount of work has been done to provide these so it is preserved as it was, but expanding available APIs is not planned.

We have code generators that produce type-safe APIs and handle freeing resources for you for the following languages:

- [Lua](https://github.com/calamity-inc/Soup/blob/senpai/bindings/soup-apigen.lua)

Otherwise, you can also just FFI without any abstractions, just be careful not to leak memory:

- [Lua](soup.lua)
- [JavaScript](docs/js-bindings-cdn.md)
- [PHP](soup.php)
- Plus, any other language that supports FFI!

For a list of available functions and their signatures, see [soup.h](https://github.com/calamity-inc/Soup/blob/senpai/bindings/soup.h).

## Building

To build Soup as a DLL/SO with C ABI exports, run `php build_lib.php` (in the root folder) or `sun dynamic` (in the "soup" folder).

For WASM, it can be done via `php wasm.php` (in the root folder) or `sun wasm` (in the "soup" folder).
