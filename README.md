# PIWCS PRW Model

This is a reference implementation of the model used to describe the Public Railway of PIWCS, a private Minecraft server.

See [include/piwcsprwmodel.h](include/piwcsprwmodel.h) for a detailed description of this library.

This project is licensed under GNU GPL version 3-or-later, see [LICENSE](LICENSE) for more details. Copyright &copy; OLEGSHA 2024.

## Usage

### Prerequisites

Only `cmake` and a C++ compiler are required to build the library. GCC and Clang are supported.

### Libraries

This project uses [minijson_reader](https://github.com/giacomodrago/minijson_reader) and [minijson_writer](https://github.com/giacomodrago/minijson_writer) (specifically, the [fork](https://github.com/ClausKlein/minijson_writer) by [ClausKlein](https://github.com/ClausKlein)) for JSON encoding and decoding.

### As a CMake subdirectory

Simply add this repository as a submodule and update it recursively, then add it as a CMake subdirectory and link against `piwcsprwmodel`:

```bash
git submodule add <clone-url> lib/piwcsprwmodel
git submodule update --init --recursive
```

```cmake
# In CMakeLists.txt
add_subdirectory(lib/piwcsprwmodel)
target_link_libraries(your_target piwcsprwmodel)
```

### As a standalone library

Clone this repository, update submodules recursively, then use CMake to build the library:

```bash
# Clone the repository
git clone <clone-url> piwcsprwmodel

cd piwcsprwmodel

# Configure the project using "build" as the build directory
cmake -B build \
    -DBUILD_SHARED_LIBS=NO # Change to YES to build as shared library

# Compile the library
cmake --build build
```

Add `piwcsprwmodel/include` to your include path. Find the compiled library in `build/libpiwcsprwmodel.a` or `build/libpiwcsprwmodel.a`.

## Documentation

Headers are documented with Doxygen comments.

To generate HTML documentation, install Doxygen, reconfigure the project and build target `doxygen`. Start browsing the HTML documentation at `<build-dir>/html/index.html`.

## Development

### Running tests

Install [GoogleTest](https://google.github.io/googletest/), reconfigure the project and build CMake target `tests` to compile unit tests. To run tests, `cd` into `<build-dir>/tests` and run `ctest`.

### Linting

Install [clang-tidy](https://clang.llvm.org/extra/clang-tidy/) version 13 or later and reconfigure the project. All targets will now run clang-tidy checks on all compiled files.

Use CMake option `SKIP_CLANG_TIDY` when configuring to disable automatic linting:
```bash
cmake -B build -DSKIP_CLANG_TIDY=ON ...
```

### Formatting

This project uses [clang-format](https://clang.llvm.org/docs/ClangFormat.html) version 13 or later for formatting. Many IDEs support `.clang-format` formatting specification out of the box.
