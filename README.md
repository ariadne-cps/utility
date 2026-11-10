# Ariadne Utility

[![macOS Status](https://github.com/ariadne-cps/utility/actions/workflows/macos.yml/badge.svg)](https://github.com/ariadne-cps/utility/actions/workflows/macos.yml)
[![Ubuntu Status](https://github.com/ariadne-cps/utility/actions/workflows/ubuntu.yml/badge.svg)](https://github.com/ariadne-cps/utility/actions/workflows/ubuntu.yml)
[![Windows Status](https://github.com/ariadne-cps/utility/actions/workflows/win.yml/badge.svg)](https://github.com/ariadne-cps/utility/actions/workflows/win.yml)
[![Sanitizers Status](https://github.com/ariadne-cps/utility/actions/workflows/sanitizers.yml/badge.svg)](https://github.com/ariadne-cps/utility/actions/workflows/sanitizers.yml)
[![Coverage Status](https://github.com/ariadne-cps/utility/actions/workflows/coverage.yml/badge.svg)](https://github.com/ariadne-cps/utility/actions/workflows/coverage.yml)
[![codecov](https://codecov.io/gh/ariadne-cps/utility/branch/main/graph/badge.svg)](https://codecov.io/gh/ariadne-cps/utility)

Ariadne Utility is a small, independently usable C++20 utility library used by Ariadne projects. Although independently maintained, this repository is also part of the [Ariadne repository structure](https://github.com/ariadne-cps/ariadne/blob/main/doc/structure.md).

## Build

Clone the repository together with its Git submodules:

```bash
git clone --recurse-submodules https://github.com/ariadne-cps/utility.git
cd utility
cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Release
cmake --build build-release --parallel --target ariadne-utility
cmake --install build-release --prefix install
```

A C++20 compiler and CMake are required.

## Tests

Build and run the C++ tests independently in Debug mode:

```bash
cmake -S . -B build-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build-debug --parallel --target tests
ctest --test-dir build-debug --output-on-failure
```

## Sanitizers

The Sanitizers workflow runs two separate Debug builds on Ubuntu with Clang:
ThreadSanitizer for data races and AddressSanitizer + UndefinedBehaviorSanitizer
for memory errors and undefined behavior.

## Coverage

Configure a separate Debug build with coverage enabled:

```bash
mkdir build-coverage
cd build-coverage
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCOVERAGE=ON
cmake --build . --parallel --target coverage
```

On Ubuntu coverage is generated with GCC/lcov. On macOS it is generated with AppleClang/LLVM coverage tools.

## Testing dependency

The pinned `testing` submodule is used only for standalone tests. The
production `utility` interface and `ariadne-utility` library do not link
against or install Testing. The compilation root selects the active Testing
checkout, whose Build gitlink must match the production Build commit.

The common setup is provided by `ariadne_setup_module(utility)` in the pinned
Ariadne Build submodule. CMake verifies the Build gitlink paths before setup;
reconfigurations reuse successful checks when the relevant gitlinks are unchanged.
