> [!WARNING]  
> This project is still in development and may be unstable.

# tmalloc
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg?style=for-the-badge)](LICENSE)

**tmalloc** is a cross-platform (Linux and Windows) C library that provides a time-based memory allocator.
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
> - Automatic freeing is best-effort and platform-dependent

## Build instructions

**tmalloc** works on both Linux and Windows and uses CMake as its build system. 
Follow these steps to build the static library:

### Requirements

- C compiler (GCC, Clang, or MSVC)
- CMake 3.15+

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

This builds the static library. You can link it manually into another project, 
or install it for use with `find_package()` as described below.

3. **Install**

    ```bash
    cmake --install build
    ```

> [!NOTE]  
> Running `cmake --install build` without `--prefix` installs to `CMAKE_INSTALL_PREFIX`.
> If you do not want the default install location use the `--prefix <path/to/dir>` flag.
> If you install to a non-default location, CMake will not search that location automatically, 
> so you will have to set `-DCMAKE_PREFIX_PATH=<path/to/install>` when creating your project.

## Example of using tmalloc in another CMake project

To use tmalloc in any CMake project see the example below:

### Install tmalloc

```bash
git clone https://github.com/Hudson111-s/tmalloc.git
cd tmalloc
cmake -S . -B build
cmake --build build
cmake --install build --prefix ~/code/project1/install
cd ~/code/project1
```

Now you should have:

```bash
~/code/project1/install
├── include/
│   └── tmalloc.h
├── lib/
│   ├── libtmalloc.a
│   └── cmake/
│       └── tmalloc/
│           ├── tmallocConfig.cmake
│           ├── tmallocConfigVersion.cmake
│           └── tmallocTargets.cmake
```

### Add to CMakeLists.txt

```cmake
find_package(tmalloc REQUIRED)

add_executable(my_program
    main.c
)

target_link_libraries(my_program PRIVATE
    tmalloc::tmalloc
)
```

### Configure and build project

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH=~/code/project1/install
cmake --build build
```

## Contribution

Contributions are very welcome! Feel free to open [issues](https://github.com/Hudson111-s/tmalloc/issues) or submit [pull requests](https://github.com/Hudson111-s/tmalloc/pulls).

## Author

Created with ❤️ by [Hudson111-s](https://github.com/Hudson111-s)
