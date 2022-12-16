# Simple generic hashtable

Simplistic and generic hashtable using the Murmur3 hash
function and linear probing to resolve collisions,
implemented in C99.

For example usage see [example.c](example.c). 

**WARNING:** this library is a naive implementation and is
not yet thoroughly tested. Use at your own risk (possibly
better, use a more widely used library e.g.
[stb_ds](https://github.com/nothings/stb/blob/master/stb_ds.h)
or [ZPL](https://github.com/zpl-c/zpl))
