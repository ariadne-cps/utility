# Ariadne Utility

[![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![Unix Status](https://github.com/ariadne-cps/utility/actions/workflows/unix.yml/badge.svg)](https://github.com/ariadne-cps/utility/actions/workflows/unix.yml)
[![Windows Status](https://github.com/ariadne-cps/utility/actions/workflows/win.yml/badge.svg)](https://github.com/ariadne-cps/utility/actions/workflows/win.yml)
[![Sanitizers Status](https://github.com/ariadne-cps/utility/actions/workflows/sanitizers.yml/badge.svg)](https://github.com/ariadne-cps/utility/actions/workflows/sanitizers.yml)
[![Coverage Status](https://github.com/ariadne-cps/utility/actions/workflows/coverage.yml/badge.svg)](https://github.com/ariadne-cps/utility/actions/workflows/coverage.yml)
[![codecov](https://codecov.io/gh/ariadne-cps/utility/branch/main/graph/badge.svg)](https://codecov.io/gh/ariadne-cps/utility)

Ariadne Utility is a small C++20 utility library used by Ariadne projects.

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

## Documentation prototype (Doxygen to Markdown)

The experimental `docs-doxygen-markdown` branch uses the Doxygen configuration
and `doc` target adapted from [Ariadne](https://github.com/ariadne-cps/ariadne).
The input is Utility's `include/` and `src/` source code; no manual API
pages or generated outputs are committed.

Requirements: Doxygen, Python 3, Node.js 20+ and Moxygen 2.1.19.
Generate the documentation with:

```sh
npm install --global moxygen@2.1.19
cmake -S . -B build-docs
cmake --build build-docs --target doc-markdown
```

The `doc` target produces Doxygen HTML (`build-docs/docs/html/`), XML
(`build-docs/docs/xml/`) and, by default, LaTeX sources
(`build-docs/docs/latex/`). The `doc-markdown` target additionally
generates Hugo-ready Markdown with YAML front matter in
`build-docs/docs/markdown/`. It cleans obsolete Markdown on each run.
Use `-DDOXYFILE_LATEX=OFF` for HTML/XML/Markdown-only builds, or build
`doc-pdf` to compile the LaTeX PDF when TeX and make are available.

The generated Markdown is an experimental API output; Hugo-specific URLs,
navigation and cross-repository links must be validated before website
publication. This branch does not alter the website or other repositories.
