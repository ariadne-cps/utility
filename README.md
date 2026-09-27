# Ariadne Utility

[![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![Unix Status](https://github.com/ariadne-cps/utility/actions/workflows/unix.yml/badge.svg)](https://github.com/ariadne-cps/utility/actions/workflows/unix.yml)
[![Windows Status](https://github.com/ariadne-cps/utility/actions/workflows/win.yml/badge.svg)](https://github.com/ariadne-cps/utility/actions/workflows/win.yml)
[![Coverage Status](https://github.com/ariadne-cps/utility/actions/workflows/coverage.yml/badge.svg)](https://github.com/ariadne-cps/utility/actions/workflows/coverage.yml)
[![codecov](https://codecov.io/gh/ariadne-cps/utility/branch/main/graph/badge.svg)](https://codecov.io/gh/ariadne-cps/utility)

Ariadne Utility is a small C++20 utility library used by Ariadne projects.

## Build

Clone the repository together with its Git submodules:

```bash
git clone --recurse-submodules https://github.com/ariadne-cps/utility.git
cd utility
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --parallel
ctest --output-on-failure
```

A C++20 compiler and CMake are required.

## Coverage

Configure a separate Debug build with coverage enabled:

```bash
mkdir build-coverage
cd build-coverage
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCOVERAGE=ON
cmake --build . --parallel --target coverage
```

On Ubuntu coverage is generated with GCC/lcov. On macOS it is generated with AppleClang/LLVM coverage tools.
