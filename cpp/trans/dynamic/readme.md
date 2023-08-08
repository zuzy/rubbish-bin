# uninitialized error #

```bash
dynamic.cc: In function ‘int FastLog2(int)’:
dynamic.cc:15:7: error: ‘f_x’ is used uninitialized [-Werror=uninitialized]
   15 |   l_x = *(unsigned int*)&(f_x);
      |   ~~~~^~~~~~~~~~~~~~~~~~~~~~~~
dynamic.cc:14:9: note: ‘f_x’ declared here
   14 |   float f_x(x);
      |         ^~~
cc1plus: all warnings being treated as errors
```

reference:

- [Optimize Options](https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html)
- [Structures, Unions, Enumerations, and Bit-Fields](https://gcc.gnu.org/onlinedocs/gcc/Structures-unions-enumerations-and-bit-fields-implementation.html)


-fstrict-aliasing

Allow the compiler to assume the strictest aliasing rules applicable to the language being compiled. For C (and C++), this activates optimizations based on the type of expressions. In particular, an object of one type is assumed never to reside at the same address as an object of a different type, unless the types are almost the same. For example, an unsigned int can alias an int, but not a void* or a double. A character type may alias any other type.

Pay special attention to code like this:

```cpp
union a_union {
  int i;
  double d;
};

int f() {
  union a_union t;
  t.d = 3.0;
  return t.i;
}
```

The practice of reading from a different union member than the one most recently written to (called “type-punning”) is common. Even with -fstrict-aliasing, type-punning is allowed, provided the memory is accessed through the union type. So, the code above works as expected. See Structures, Unions, Enumerations, and Bit-Fields. However, this code might not:

```cpp
int f() {
  union a_union t;
  int* ip;
  t.d = 3.0;
  ip = &t.i;
  return *ip;
}
```

Similarly, access by taking the address, casting the resulting pointer and dereferencing the result has undefined behavior, even if the cast uses a union type, e.g.:

```cpp
int f() {
  double d = 3.0;
  return ((union a_union *) &d)->i;
}
```

__The -fstrict-aliasing option is enabled at levels -O2, -O3, -Os.__
