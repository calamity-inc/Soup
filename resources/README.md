# Soup Resources

## jokebook.bin

```C++
HttpRequest hr(Uri("https://raw.githubusercontent.com/Sainan/official_joke_api/master/jokes/index.json"));
auto res = hr.execute();
auto jr = json::decodeForDedicatedVariable(res->body);
Jokebook jb;
jb.loadJson(jr->asArr());
FileWriter fw("jokebook.bin");
jb.write(fw);
```
