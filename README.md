# Description

- A place to store all of my single header libraries

| File   | Purpose                                                                             | Dependency      |
|--------|-------------------------------------------------------------------------------------|-----------------|
| fsv.h  | A simple string view library for string manipulation without allocating more memory | `libc`          |
| flog.h | A simple logging library                                                            | `libc`          |
| fcsv.h | A parser for `csv` file                                                             | `libc`, `fsv.h` |

# How to use

- Just download the file you need to use and `#define F{library_all_uppercase}_IMPLEMENTATION` macro
  for such file in *one* C/C++ file to create the implementation.

```c

// #include ...
#define FSV_IMPLEMENTATION
#include "fsv.h"
// #include ...

int main(void) {
    fsv_t right = fsv_from_cstr("Use theses [library] at your own risk");
    fsv_t mid, left;
    fsv_split_by_pair(&right, "[]", &mid, &left);

    printf("left  = " fsv_fmt "\n", fsv_arg(left));  // "Use theses "
    printf("mid   = " fsv_fmt "\n", fsv_arg(mid));   // "library"
    printf("right = " fsv_fmt "\n", fsv_arg(right)); // " at your own risk"

    return 0;
}

```

