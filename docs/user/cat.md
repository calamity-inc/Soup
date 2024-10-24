# Colons and Tabs format (CaT)

CaT is a dead-simple configuration format that arose from our need for a human-friendly & easy-to-parse data exchange format.

The most important aspect about it is how easy it is to generate valid CaT from any pre-existing tree structure.

A tree structure you may be familiar with is the filesystem. For example, if we write "Hello, world!" at "/home/john/hello.txt", we have the node "home" with the child "john" with the child "hello.txt" with the value "Hello, world!". This could be represented in CaT as follows:

```
home
    john
        hello.txt: Hello, world!
```

Note that, on this page, 4 spaces are used instead of actual tab characters to ensure consistent rendering, although [this is also valid CaT](#spaces).

## Files

We recommend that files storing CaT data use the `.txt` extension because it opens in the user's default text editor without any hassle.

If you also want it to be clear that the file uses the CaT format, we recommend using the `.cat.txt` extension.

## Architecture

CaT only recognises "nodes," which can have a name (string), value (string), and children (vector of node).

Strings are the only value type because they can be easily converted to and from bool, int, float, and even custom types.

## Name-Value Pairs

Names and values are separated by a colon followed by a space (`: `). A mere colon (`:`) is only allowed if there is no value.

```
Subject: Hello
To: World
```

A node without a value doesn't need a colon at all:

```
With Value: Yay!
Without Value
Explicitly Without Value: 
```

The name of a node can also contain colons, in which case they MUST be escaped with a backslash:

```
Colons (\:): Check!
```

The name may also be empty, in which case a colon is required to flag the existence of the node:

```
Nameless
    : Like This
Nameless And Valueless
    :
```

## Value Encoding

In general, a value can simply be written as-is, but to prevent data loss or corruption, special handling SHOULD be performed for values that contain `"`, `\r`, or `\n` characters: Add a backslash (`\`) before each of these characters, as well as each `\` present in the source value, and wrap the value in quotes (`"`), for example:

```
Multi-line: "Several readers remarked that \"it's colons and tabs time\" and then put colons and tabs all over the place.\r\n\r\nContainment procedures are still being developed."
```

In this case, we have a single child node called "Multi-line" with a value of:

```
Several readers remarked that "it's colons and tabs time" and then put colons and tabs all over the place.

Containment procedures are still being developed.
```

When decoding, such values wrapped in quotes (`"`) SHOULD be detected, unwrapped, and have the escape sequences resolved:
- `\\n` -> `\n`
- `\\r` -> `\r`
- `\"` -> `"`
- `\\` -> `\`

## Children

CaT uses indentation to determine which node is the parent of a newly-encountered node:

```
Colons: Check!
    Tabs: Check!
Running Out Of Placeholders: Check!
```

In this case, the nodes named "Colons" and "Running Out Of Placeholders" are children of the root node, and "Tabs" is a child of the "Colons" node.

Note that once the "Running Out Of Placeholders" node began, the "Colons" node was finished. A new node with the same name would not affect this one, for example:

```
Colons: Yes
    Tabs: Of Course
Colons: Duh
```

Here, we have two nodes called "Colons" with the values "Yes" and "Duh", respectively.

An empty line (even with indentation) must be ignored.

## Spaces

A CaT parser SHOULD handle spaces as follows:
- On the first occurrence of a space character: Count the number of space characters in a row and remember it as the "baseline count." Treat this as if a single tab character was written instead of the space characters.
- On the next occurrence of a space character: Count the number of space characters in a row and each time it reaches the "baseline count," treat it like a single tab character.
