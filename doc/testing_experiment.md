# Utility Testing migration experiment

## 2026-10-30 — Independent framework experiment

This journal describes the `testing` branch of `ariadne-cps/utility`, created
afresh from `utility/main`. Do not apply experimental changes directly to
`main` before review and CI validation.

### Dependency separation

- Runtime / installed Utility: depends on Configuration only.
- Legacy Macro repository: no longer a dependency; its production definitions
  from `macro/general.hpp` live in `include/utility/macros.hpp`. Utility
  headers now include the new local header.
- Test-only dependency: `ariadne-cps/testing` pinned at `ss/tst`, intentionally
  excluded from the production `utility` INTERFACE target.

CMake's `setup_project_library` does not create an installable library
when the framework is consumed as a nested subproject; its `TESTING_SRC`
object target is linked into `utility_testing_runtime`, a private static
archive in the tests directory. All ten existing standalone CTest executables
link `utility_testing_runtime`, Utility and the Testing interface.

### Semantic migration

The previous `macro/testing.hpp` calls were migrated to `Ariadne::Testing`
without removing any original test functions. Legacy `_ASSERT` and
`_EQUALS` become `Context::prove` and `expect_equal`; expected exceptions
are checked via `expect_exception` or `expect_throws<E>`; operations not
expected to throw use `expect_no_throw`. Diagnostic printing stays on
`std::cout` and exercises the original values. Each original CTest executable
remains separate.

The old dedicated Macro testing self-test is replaced by assertions checking
`Truth::True`, `Truth::False`, `Truth::Indeterminate` and the ADL domain
customizations (`same`, `mag`, validated comparisons); an inconclusive
property is deliberately reported as `Inconclusive` rather than warning-only.

The dependency remains test-only, even though standalone Utility CMake
initializes the pinned Testing gitlink to compile its tests.

### Integration incidents and corrections

1. The initial source rewrite introduced malformed multiline syntax. Corrected
   all seven CLI cases without removing test scenarios.
2. Nested `setup_project_library` does not export `ariadne-testing` as a
   CMake library target. Avoid linking a nonexistent `-lariadne-testing`;
   build a test-only static library from `TESTING_SRC` instead.
3. Generic equality of signed versus unsigned integral types triggered
   `-Wsign-compare -Werror`; Testing uses `std::cmp_equal` for pairs of
   built-in non-bool integral types, preserving custom validated comparisons.
4. The framework signedness self-test initially used a narrowing list
   initialization. Replaced with an explicit cast.

### Verification and future work

All ten isolated test builds and executions passed on Ubuntu GCC 13 after
these fixes. Ubuntu, macOS, and Sanitizers workflow families passed for
Utility commit `03d6c075`, while the Windows ARM Debug job failed; it
requires follow-up verification. The newer Testing commit `c47c407` is
pinned here. Production changes are intentionally not applied to
`utility/main`.

Future iterations should prefer direct `Context&` injection and named suite
cases over the temporary fixture-pointer bridge, and add parameterized tests
only when their execution semantics are established. Do not weaken tests to
make CI green.


## 2026-10-30 — Individual function cases and numbered checks

Each original `test_*` function is now registered as its own named
`Suite::add` case in declaration order (33 functions across eight existing
test source files, plus the existing dedicated semantics and typedef suites).
The old `legacy scenarios` wrapper and redundant exception-catching
dispatch have been removed. Cases normally invoke the function directly;
demonstration-only functions (printing, stack tracing, creation) use a single
`expect_no_throw` checker to record a real operational observation rather than
silently being classified as skipped.

Progress is tracked by the Testing framework, not by extra print statements.
The console reporter emits and flushes `[RUN i/N] suite / function` before
the function and `[CHECK i.j] description` for each checker; j resets to
1 at the next function. There is no third hierarchy level. The Testing gitlink
is pinned to the implementation and its regression tests.

**Limitation:** C++ evaluates checker arguments before entering eager
checkers such as `expect_equal`. Their CHECK event cannot precede hangs
inside argument evaluation. For expensive expressions use deferred
`Context::check([&] { return expression; }, "description")`.
Exception checkers are already deferred. Separate CTest executables each
have their own suite and numbering; no global cross-process index exists.

### 2026-10-30 — Correct accidental removal of CLI test methods

An overly broad dispatcher-removal pattern removed the seven `Void test_*`
method definitions in `tests/test_command_line_interface.cpp` while keeping
the new per-function suite registrations. Clang therefore reported seven
missing members. The seven method bodies have been restored **verbatim**
from Utility commit `2e80df3f0f9f489147cc3c6587c1330df96766a5`; only the old `Void test()` dispatcher
is omitted. Method declarations and the seven `Suite::add` invocations
were compared in original order to prevent coverage loss. Added the explicit
`<iostream>` include for `std::cout`. CI must confirm compilation and
execution before this branch is promoted.

## 2026-10-30 - Compact fixture registration

All 33 test-function registrations across eight test executables now use `ARIADNE_ADD_TEST(suite, Fixture, test_method)` or `ARIADNE_ADD_TEST_NO_THROW` for demonstration-only methods. The method name is written once. The C++20 helper in the Testing repository validates member function pointers, constructs a fresh fixture from Context&, and preserves case/check numbering and errors. The macro only stringifies a token.

Fixtures now derive from `ContextFixture` and inherit its constructor instead of storing a nullable `_ctx` pointer. TestRandomiser preserves ten attempts through its context constructor. No test-method body was removed. The pinned Testing gitlink includes the new helper. CI confirmation is required.

## 2026-10-30 — Source headers

Added the standard filename/copyright/author and GPLv3-or-later notices to new sources lacking them. Corrected the copied `utility/macros.hpp` header that incorrectly mentioned the old Macro repository and filename. Existing authorship headers for the other Utility files are preserved. Changes are comments only.

## 2026-10-30 — Update Testing submodule to D01–D04 and remove duplicate test

The test-only `ss/tst` gitlink now targets Testing main at `87dc260218298998e404c98bf541a470472c0dd5`,
including the expression/predicate/evidence checks, parameterized fixture
variants and nested diagnostic contexts. Existing Utility test bodies and
assertion calls were deliberately not rewritten during this dependency
update; this ensures that any CI difference measures compatibility with
the newer Testing API rather than a simultaneous semantic migration.

Per request, removed `tests/test_testing.cpp`, which independently tested
the Testing framework's validated truth/identity/tolerance semantics;
Testing's own CTest self-tests cover these framework concerns. Removed
`test_testing` from `tests/CMakeLists.txt` and its `tests` aggregate
via `UNIT_TESTS`. The Utility-specific test executables remain untouched;
there are now nine standalone CTest executables in Utility's test directory.
No changes to Utility's production library or the `main` branch.
This operation requires validation in Utility's CI after the submodule bump.

## 2026-10-30 — Verify Testing submodule update after transient CI failures

The initial Utility CI at commit `3491651` reported compilation failures across
platforms. A temporary diagnostics workflow was added at `6e80688` to expose
the first build error. The diagnostics Release build succeeded, and the
complete Ubuntu, macOS, Windows and Sanitizers workflow families subsequently
passed on that commit with the same `ss/tst` gitlink. Therefore no code or
expectation change was justified by the evidence. The temporary workflow
is removed. The original failures were not conclusively diagnosed, so
repeated CI failures should be investigated independently if they recur.

## 2026-10-30 — Replace repetitive Context assertion calls with expression macros

The existing test suites on branch `testing` now use the optional assertion
macros in `testing/checks.hpp`: `ARIADNE_PROVE`,
`ARIADNE_EXPECT_EQUAL`, `ARIADNE_EXPECT_EXCEPTION`,
`ARIADNE_EXPECT_THROWS` and `ARIADNE_EXPECT_NO_THROW`.
The original method bodies were migrated by replacing only the assertion
call sites and including the macro header; no test case was removed, no
operation or input was altered, and the 33 fixture-method registrations
remain unchanged. The standalone `test_typedefs.cpp` is migrated too.
The method/lambda factory registration in `testing/fixture.hpp` remains
unchanged, and demonstration-only tests still use `ARIADNE_ADD_TEST_NO_THROW`.

In the new form, expression descriptions come from preprocessor source
stringification, not manually duplicated string arguments. Comparison
and proof operands are evaluated once *after* the numbered CHECK event,
which improves hang diagnostics; exception operations remain deferred.
Only the displayed check descriptions and the evaluation point relative
to CHECK events change. Do not mechanically substitute richer
semantics such as `SAME` or `WITHIN` into equality: these tests retained
their original checker operations. The production Utility library and
the `main` branch are unchanged.

Source-level migration check: 118 check calls converted
across 7 test files;
each source retains the original number of `test_*` methods.
CI must verify compilation/execution on all target platforms.


## 2026-10-30 — Adopt fixture shorthand and conventional suites

The experimental `utility/testing` branch now pins the test-only `ss/tst`
submodule at Testing commit `044139bf5873744a6c1bead10e67677e1e90a684`.
This dependency revision passed Testing's Ubuntu, macOS, Windows,
Sanitizers and Coverage CI workflows.

The eight fixture-based test executables now use implicit assertions such
as `ARIADNE_EXPECT_EQUAL(actual, expected)`, internally forwarding to
`*_CTX(this->ctx(), ...)`, rather than explicitly passing `ctx()`.
Their `main()` functions use `ARIADNE_TEST_SUITE(name)`,
`ARIADNE_ADD_TEST(Fixture, method)` (or the `_NO_THROW` variant), and
`return ARIADNE_RUN_TESTS();`. The non-fixture `test_typedefs.cpp`
continues to pass the lambda-local context explicitly via
`ARIADNE_EXPECT_EQUAL_CTX(ctx, actual, expected)`; it also uses the
conventional suite creation/run macros.

This is exclusively an API-syntax migration: 118 assertion-call sites,
33 fixture-method registrations with unchanged ordering, nine independent
CTest executables, all original test bodies, exception checks and explicit
NO_THROW demonstration checks are retained. The test-only dependency does
not leak into the production library. No changes are made to `utility/main`.
Utility's own CI must confirm the new revision before promotion.


## 2026-10-30 — Umbrella include and strictly test-only dependency

All nine Utility test translation units now include only
`ariadne-testing.hpp` for their Testing framework APIs. This single
header collects the core types, ContextFixture, assertion macros and
parameterized variants; it does not replace unrelated Utility or C++
standard library includes. The test-only `ss/tst` gitlink is updated to
Testing `9e41df5496e1fe4d26d14f62300f3609d71c03a2`.

Testing intentionally has **no install rules**: the header is available
only in the source/build tree and its implementation is linked only to
test executables. Utility's production target, install bundle, and
`utility/main` remain untouched. No checker expressions, test case bodies,
registrations, assertions or CTest definitions have been modified.
Utility CI must validate this dependency update before promotion.

## 2026-10-30 — Centralized test-only CMake integration

Pinned `ss/cfg` to Configuration `a3367703c0f95af09b823560f19c6830aaa760fc`
and `ss/tst` to Testing `de5e3200c78397db2f3db42e63aa3c182e7824ec`.
The root `setup_standalone_project_tests` invocation now passes the
`USE_ARIADNE_TESTING` opt-in. Configuration initializes the pinned Testing
submodule only for standalone tests, before entering `tests/`. The nine
CTest executables link the existing build-tree `ariadne-testing` static
library rather than the redundant `utility_testing_runtime` archive.
Testing remains excluded from Utility's production interface, build and
install bundle. Test sources, case registrations, check expressions,
expected outcomes and CTest inventory are unchanged.
