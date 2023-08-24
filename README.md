A very basic regex parser developed as a programming exercise.
Supports the POSIX ERE syntax.
For now, the main function is located at `src/parser/api.h`

# Build instructions

```
mkdir build
cd build
cmake /path/to/regex-parser/ -DCMAKE_BUILD_TYPE=DEBUG
make
```

In order to perform checks with clang-tidy, append  `-DPERFORM_STATIC_CHECKS=ON`
to the `cmake` command line.

# Running unit tests

## Without coverage
Run inside the build directory:
```
GTEST_COLOR=1 ctest -V
```

## With coverage
Append `-DCODE_COVERAGE=ON` to the `cmake` command line and run:
```
make coverage
```
