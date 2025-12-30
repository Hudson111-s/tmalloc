> [!WARNING]  
> This project is still in development and may be unstable.


# tmalloc
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg?style=for-the-badge)](LICENSE)


**tmalloc** is a C library that provides a time-based memory allocator.
It behaves like `malloc`, except allocated memory is automatically freed
after a specified lifetime (in milliseconds). Trading safety for convenience and control.


## When should you use it?

**tmalloc** is useful when memory lifetimes are naturally time-bounded:

- Temporary buffers
- Game / real-time engine allocations
- Networking code (request buffers, timeouts, session state)
- Prototyping or experimental systems


## When should you avoid it?

**tmalloc** is not recommended for:

- Long-lived variables or global states
- Public APIs that expose tmalloc pointers
- Safety-critical systems
- Code that assumes pointers remain valid indefinitely
- Replacing malloc


## Usage

```c
#include "tmalloc.h"

int *value = tmalloc(sizeof(int), 5000); // valid for ~5 seconds
*value = 42;

printf("%d\n", *value);

// Optional manual free.
tfree(value);
```

See [`tmalloc.h`](include/tmalloc.h) for documentation.

> [!NOTE]
> - All functions are thread safe
> - Spawns a single background reaper thread on first use
> - Automatic freeing is best-effort and platform-dependent, averages ~1ms accuracy


## Build instructions

**tmalloc** uses CMake to build, please follow the following steps to build the static library:

### Requirements

- C compiler (GCC, Clang, or MSVC)
- CMake 3.10+

### Build steps

1. **Clone the repository**

    ```bash
    git clone https://github.com/Hudson111-s/tmalloc.git
    cd tmalloc
    ```


2. **Build**

    ```bash
    cmake -S . -B build
    cmake --build build
    ```


## Contribution

Contributions are very welcome! Feel free to open [issues](https://github.com/Hudson111-s/tmalloc/issues) or submit [pull requests](https://github.com/Hudson111-s/tmalloc/pulls).


## Author

Created with ❤️ by [Hudson111-s](https://github.com/Hudson111-s)
