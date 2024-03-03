# STL Drop-in Replacements

Soup provides a few alternatives for STL types, functions, and templates — each with their own pros and cons.

- UniquePtr — No RTTI & no deleter = less overhead. Still works fine with inheritance due to virtual dtors.
- SharedPtr — Same benefits as UniquePtr + release method (requires caller to be only one with ref).

## Polyfills

The following are provided to allow usage of C++ 20 features without dropping C++ 17 support:

- memory.hpp: construct_at
- spaceship.hpp: strong_ordering & SOUP_SPACESHIP
