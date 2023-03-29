# Programming a language

Say you have a syntax in mind, such as:

```PHP
echo "Hello world!";
```

This document will explain how to go from this text to executing the logic. We will go over the following steps:

- Tokenising (aka. lexing)
- Parsing
- Compiling (to VM instructions)
- Executing

## Syntax Highlighting

Let's start by creating a simple `soup::LangDesc` that recognises the `echo` keyword and assigns it the colour of blue, then we just call `highlightSyntax` on it, and print it to the console:

```C++
#include <soup/console.hpp>
#include <soup/FormattedText.hpp>
#include <soup/LangDesc.hpp>

int main()
{
    soup::LangDesc ld;
    ld.addToken("echo", soup::Rgb::BLUE);

    soup::console.init(false);
    soup::console << ld.highlightSyntax(R"(echo "Hello, world!";)").toString() << "\n";
    soup::console.resetColour();
}
```

This code will print a blue `echo`, a yellow `"Hello, world!"`, and a white `;`.

What just happened here? Let's take a look at the implementation of `highlightSyntax`:

```C++
FormattedText LangDesc::highlightSyntax(const std::string& code) const
{
    return highlightSyntax(tokenise(code));
}
```

Aha, we have actually just done the step of tokenisation, and it just so happens that `LangDesc::tokenise` has `""`-strings built-in.

Now, let's move on to parsing.

## Parsing

This step is all about building an abstract syntax tree (aka., parse tree). We can do this by adding a simple parse function to our call to `addToken`:

```C++
#include <soup/console.hpp>
#include <soup/FormattedText.hpp>
#include <soup/LangDesc.hpp>
#include <soup/Lexeme.hpp>
#include <soup/ParserState.hpp>
#include <soup/parse_tree.hpp> // astBlock

enum MyOpCodes : int
{
    OP_ECHO = 0,
};

int main()
{
    soup::LangDesc ld;
    ld.addToken("echo", soup::Rgb::BLUE, [](soup::ParserState& ps)
    {
        ps.setOp(OP_ECHO);
        ps.consumeRighthandValue();
    });

    std::vector<soup::Lexeme> ls = ld.tokenise(R"(echo "Hello, world!";)");
    soup::console.init(false);
    soup::console << ld.highlightSyntax(ls).toString() << "\n";
    soup::console.resetColour();

    soup::astBlock root = ld.parse(ls);
    soup::console << root.toString() << "\n";
}
```

Now we've still got a syntax highlighter but the tokenisation is done explicitly, we're now building a parse tree, and already have an OpCodes enum for later.

The program now also prints the AST, which in our case will look something like this:

```
block
    op 0
        value: Hello, world!
```

## Compiling

Since we have an AST now, we can simply call `ast.compile`, which takes a `soup::Writer` to, unsurprisingly, feed the bytecode to. For now, we'll simply write it to a string.

```C++
#include <soup/StringWriter.hpp>
```
```C++
soup::StringWriter w;
ast.compile(w);
```

The bytecode format is specific to Soup, but not too extravagant. With the AST from before, it will look like this:

```
OP_PUSH_STR "Hello, world!"
OP_ECHO
```

## Executing

All that's left to do now is writing a virtual machine using `soup::LangVm` so we'll only have to worry about our own op codes:

```C++
#include <soup/LangVm.hpp>
```
```C++
static void myVm(soup::Reader& r)
{
    soup::LangVm vm{ r };
    for (uint8_t op; vm.getNextOp(op); )
    {
        switch (op)
        {
        case OP_ECHO:
            std::cout << vm.pop()->toString() << std::endl;
            break;
        }
    }
}
```

To invoke it with our compiled bytecode from before, we can simply do:

```C++
#include <soup/StringReader.hpp>
```
```C++
soup::StringReader r{ std::move(w.data) };
myVm(r);
```

And that's it! Our program now prints `Hello, world!`.

For further reading, check out `soup::PhpState`, which uses Soup's language stack for a minimalistic PHP reimplementation.

## The complete program

```C++
#include <soup/console.hpp>
#include <soup/FormattedText.hpp>
#include <soup/LangDesc.hpp>
#include <soup/LangVm.hpp>
#include <soup/Lexeme.hpp>
#include <soup/ParserState.hpp>
#include <soup/parse_tree.hpp> // astBlock
#include <soup/StringReader.hpp>
#include <soup/StringWriter.hpp>

enum MyOpCodes : int
{
    OP_ECHO = 0,
};

static void myVm(soup::Reader& r)
{
    soup::LangVm vm{ &r };
    for (uint8_t op; vm.getNextOp(op); )
    {
        switch (op)
        {
        case OP_ECHO:
            std::cout << vm.pop()->toString() << std::endl;
            break;
        }
    }
}

int main()
{
    soup::LangDesc ld;
    ld.addToken("echo", soup::Rgb::BLUE, [](soup::ParserState& ps)
    {
        ps.setOp(OP_ECHO);
        ps.consumeRighthandValue();
    });

    std::vector<soup::Lexeme> ls = ld.tokenise(R"(echo "Hello, world!";)");
    soup::console.init(false);
    soup::console << ld.highlightSyntax(ls).toString() << "\n";
    soup::console.resetColour();

    soup::astBlock root = ld.parse(ls);
    soup::console << root.toString() << "\n";

    soup::StringWriter w;
    root.compile(w);

    soup::StringReader r{ std::move(w.data) };
    myVm(r);
}
```
