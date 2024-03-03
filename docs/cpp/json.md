# JSON

To use Soup's JSON capabilities you only need to include json.hpp.

```cpp
#include <soup/json.hpp>
```

## Decode

```cpp
if (auto root = soup::json::decode("[1, 2, 3]"))
{
    // root->isArr(); - true
    // root->asObj(); - Would throw because of type mismatch
    for (const auto& elm : root->asArr())
    {
        std::cout << elm.asInt() << "\n"; // Prints 1, 2, & 3 on separate lines.
    }
}
else
{
    // String failed to parse
}
```

## Encode

```cpp
auto root = soup::make_unique<JsonObject>();
root->add("bool", true);
root->add("int", 1337);
{
    auto arr = soup::make_unique<JsonArray>();
    arr->children.emplace_back(soup::make_unique<JsonString>("string in an array"));
    root->add("arr", std::move(arr));
}
std::cout << root->encode() << "\n"; // {"bool":true,"int":1337,"arr":["string in an array"]}
std::cout << root->encodePretty() << "\n"; // Same thing but with more '\n' and ' '
```
