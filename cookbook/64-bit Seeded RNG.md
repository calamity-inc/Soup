# 64-bit Seeded RNG

A common algorithm for seeded random number generators is the linear congruential generator. For example, this is the algorithm used for `srand` + `rand`.

Soup provides a 64-bit version of this algorithm with higher entropy and reasonable default parameters under `soup::algLcgRng`.

## An example

```C++
#include <iostream>
#include <soup/algLcgRng.hpp>

int main()
{
	soup::algLcgRng rng(1337);
	std::cout << rng.generate();
}
```

This will always print 6103271329652017496.
