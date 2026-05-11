# Modern C++ Best Practices

> Compiled from: LearnCpp.com · StudyPlan.dev (Intro, Pro C++, CMake) · C++ Core Guidelines (Stroustrup & Sutter) · Google C++ Style Guide · Herb Sutter & Andrei Alexandrescu _C++ Coding Standards_ · cppreference.com

---

## Table of Contents

1. [Language Standard & Compiler Setup](#1-language-standard--compiler-setup)
2. [Code Style, Naming & Formatting](#2-code-style-naming--formatting)
3. [Variables, Types & Initialization](#3-variables-types--initialization)
4. [Memory Management](#4-memory-management)
5. [Functions & Interfaces](#5-functions--interfaces)
6. [Classes & Object-Oriented Design](#6-classes--object-oriented-design)
7. [Error Handling & Exceptions](#7-error-handling--exceptions)
8. [Templates & Generic Programming](#8-templates--generic-programming)
9. [Concurrency & Parallelism](#9-concurrency--parallelism)
10. [Performance & Optimization](#10-performance--optimization)
11. [Project Structure & Build System (CMake)](#11-project-structure--build-system-cmake)
12. [Testing](#12-testing)
13. [Tooling & Static Analysis](#13-tooling--static-analysis)
14. [Deployment & CI/CD](#14-deployment--cicd)
15. [Maintenance & Documentation](#15-maintenance--documentation)

---

## 1. Language Standard & Compiler Setup

- Target **C++17 at minimum**; prefer C++20 or C++23 for new projects to unlock concepts, ranges, modules, coroutines, and `std::format`.
- Explicitly set the language standard in your build system (e.g., `set(CMAKE_CXX_STANDARD 20)` in CMake) so it never falls back silently to an older standard.
- **Disable non-standard compiler extensions** (`-pedantic` / `set(CMAKE_CXX_EXTENSIONS OFF)`) to keep code portable across GCC, Clang, and MSVC.
- Enable the **highest practical warning level**: `-Wall -Wextra -Wconversion -Wsign-conversion -Weffc++` (GCC/Clang) or `/W4` (MSVC).
- **Treat warnings as errors** (`-Werror` / `/WX`) so warning debt never accumulates; if a warning must be suppressed, document it explicitly with `[[gsl::suppress(...)]]`.
- Maintain a **separate debug build** (no optimisations, full debug info) and a **release build** (`-O2`/`-O3`, `NDEBUG`) , never ship debug binaries.
- Know and check **which language standard your compiler is actually using** before relying on new features.
- Ensure your compiler supports the features you use; consult cppreference.com "Compiler support" tables when targeting older toolchains.

---

## 2. Code Style, Naming & Formatting

- Always use (m\_) prefix for private member variables.
- Do not uses mdash (--) in the code or documentation or comments.
- Use snake_case for variable and function names and PascalCase for class and struct names.
- Always use std::print or std::println and std::format for output and formatting.
- All Variables, Constants, Free Functions, Member Functions, Member Variables and other symbols must be at least 3 characters long.
- Use a **consistent, team-agreed naming convention** and enforce it with `clang-format`; commit the `.clang-format` file to version control.
- Prefer **clear, descriptive names** over abbreviations , code is read far more often than it is written.
- Give **every entity (variable, function, class, namespace, module) one well-defined responsibility** and name it accordingly.
- Use `snake_case` for variables and functions, `PascalCase` for types and classes, `SCREAMING_SNAKE_CASE` for macros and compile-time constants (or `constexpr` values instead of macros entirely).
- Keep **functions short and flat**: a function that fits on one screen is easier to read, test, and reason about; nested code blocks are usually a sign of insufficient decomposition.
- Avoid **magic literals** (`42`, `3.14159`) in code; replace them with named `constexpr` values or symbolic expressions such as `width * aspectRatio`.
- Declare variables **as close to their point of first use as possible** , this minimises scope, reduces accidental re-use, and makes intent obvious.
- Never leave **unused variables, parameters, or includes** in committed code; they signal incomplete refactoring and waste reader attention.
- Prefer **`#pragma once`** (universally supported) or `#ifndef`/`#define` header guards; never omit them.
- Limit **header file responsibilities** to declarations and inline/template definitions only; keep definitions in `.cpp` files to reduce compilation coupling.

---

## 3. Variables, Types & Initialization

- **Always initialise variables** at the point of declaration , uninitialised variables are among the most common sources of undefined behaviour in C and C++.
- Prefer **brace (uniform) initialisation** `T x{value}` over `T x = value` or `T x(value)` because it prevents narrowing conversions and is consistently applicable.
- Use **`auto`** when the type is obvious from the right-hand side, long/verbose, or irrelevant to the reader; but spell the type out explicitly when it aids comprehension.
- Prefer **`const` by default** for all values that do not need to change , immutable values are easier to reason about and enable compiler optimisations.
- Use **`constexpr`** instead of `const` for values that are known at compile time; `constexpr` is evaluated at compile time and participates in template arguments.
- Prefer **`constexpr` functions** over macros for compile-time computations.
- Avoid **`using namespace std;`** in header files and minimise its use in source files; always qualify standard library names explicitly in headers.
- Use **strongly-typed enumerations** (`enum class`) rather than plain `enum` to avoid implicit integer conversions and name-space pollution.
- Prefer **`std::string_view`** over `const std::string&` for read-only string parameters to avoid unnecessary allocations.
- Never use **C-style casts** (`(T)x`); use `static_cast`, `reinterpret_cast`, `const_cast`, or `dynamic_cast` explicitly so intent is auditable.
- Do not mix **signed and unsigned integer arithmetic**; if you need unsigned arithmetic (e.g., bit manipulation), be deliberate and explicit.
- Avoid **overflow and underflow**: check arithmetic bounds where the inputs are untrusted; consider `<numeric>` utilities or safe integer libraries.

---

## 4. Memory Management

- Follow the **RAII (Resource Acquisition Is Initialisation) idiom**: tie every resource (memory, file handle, mutex, socket) to an object's lifetime so that destructors release it automatically.
- **Prefer stack allocation over heap allocation** whenever the size and lifetime of an object are known; stack allocation is faster and has automatic cleanup.
- Use **`std::unique_ptr`** for exclusive ownership of dynamically allocated objects; it has zero overhead over a raw pointer and encodes ownership in the type system.
- Use **`std::shared_ptr`** only when true shared ownership is required; shared ownership has reference-counting overhead and can hide lifetime design flaws.
- Use **`std::weak_ptr`** to break reference cycles in `shared_ptr` graphs; a `weak_ptr` does not affect the reference count.
- Always create smart pointers with **`std::make_unique<T>()`** or **`std::make_shared<T>()`** , never call `new` directly, as it can leak in the presence of exceptions.
- **Never `delete` a raw pointer manually** in application code; if a raw pointer appears, it should be non-owning (an observer) and its documentation must say so.
- Follow the **Rule of Zero**: if you design a class whose members are all RAII types, you need not declare a destructor, copy constructor, or copy/move assignment operator , the compiler-generated defaults are correct.
- Follow the **Rule of Five** when you do own a raw resource: declare the destructor, copy constructor, copy assignment, move constructor, and move assignment operator together.
- Use **move semantics** (`std::move`, `&&`) when transferring ownership of large objects to avoid unnecessary deep copies; never use `std::move` on a `const` object.
- Prefer **standard library containers** (`std::vector`, `std::map`, `std::unordered_map`, etc.) over manual dynamic arrays , they are RAII-compliant and use allocators correctly.
- **Never allocate more than one resource in a single statement**; split multi-resource allocations across separate lines to keep exception safety.
- Detect memory leaks during development with **AddressSanitizer** (`-fsanitize=address`) or Valgrind; treat every reported leak as a defect.
- Never use Raw pointers and C-style arrays unless dealing with legacy code or specific performance requirements.
- Never use `new` and `delete` directly in application code; use smart pointers or other RAII mechanisms instead.

---

## 5. Functions & Interfaces

- Alwayse use trailing return types for functions and methods declarations and definitions.
- Design **functions to perform one logical operation**; a function that "does two things" should probably be two functions.
- Prefer **passing by `const` reference** (`const T&`) for non-trivial types you don't need to modify; pass by value for types you will copy or move from.
- Return **values by value** and rely on Named Return Value Optimisation (NRVO) / Return Value Optimisation (RVO) rather than output parameters where possible.
- Use **`[[nodiscard]]`** on functions whose return values must not be silently discarded (e.g., error codes, smart pointers, `expected<T, E>`).
- Mark functions that **cannot throw** as `noexcept`; this enables optimisations and is part of the function's contract.
- Prefer **default function arguments** sparingly; too many defaults hide what a caller is actually providing , consider overloads or a builder pattern instead.
- Avoid **output parameters** (pointer/reference arguments used only to return values); prefer returning a `struct`, `std::pair`, or `std::tuple` instead.
- Use **lambda expressions** for short, local callbacks; capture only what is needed and avoid capturing `[=]` when the captured set is large.
- Prefer **`std::function`** for type-erased callbacks only when necessary; for performance-sensitive code, prefer templates with concepts constraints instead.
- Validate **function preconditions** with `assert()` in debug builds or `[[expects]]` contracts (C++26); document them in comments if not expressible in code.
- Never return a **reference or pointer to a local variable** , it is undefined behaviour; return by value or by smart pointer.

---

## 6. Classes & Object-Oriented Design

- Make **class invariants explicit**: the constructor should establish the invariant and every public method should preserve it.
- Prefer **concrete value types** over class hierarchies when polymorphism is not needed; value types compose better, are easier to test, and avoid virtual dispatch overhead.
- Declare **destructors of base classes `virtual`** to ensure correct destruction through a base pointer; or declare them `protected` and non-virtual to forbid deletion through the base.
- Mark overriding functions with **`override`** (and `final` where appropriate) so the compiler catches silent shadowing bugs when a base class signature changes.
- Declare **single-argument constructors `explicit`** unless implicit conversion is intentional and documented.
- Keep class members **private by default**; expose only what is needed through a minimal, stable public interface.
- Use **`struct`** for plain data aggregates with no invariants and all-public members; use `class` when invariants must be enforced.
- Prefer **composition over inheritance** for code reuse; use inheritance only to model true "is-a" relationships or to implement interfaces.
- Prefer **pure abstract interfaces** (all pure virtual functions, no data members) over deep concrete hierarchies.
- Do not call **virtual functions from constructors or destructors** , derived overrides are not yet in effect during base construction.
- Avoid **implicit type conversions** through operator overloads or constructors unless they are natural and well-understood by callers.
- Use **`[[nodiscard]]`** on getter-like member functions to prevent callers from accidentally ignoring the returned state.

---

## 7. Error Handling & Exceptions

- **Use exceptions for error handling** unless you are writing hard-real-time code or a codebase that prohibits exceptions; do not return error codes from constructors.
- Follow a consistent **exception-safety guarantee**: either basic (no resource leaks), strong (operation either succeeds completely or rolls back), or no-throw.
- **Catch exceptions by `const` reference** (`catch (const std::exception& e)`), never by value, to avoid object slicing.
- Throw **specific exception types** derived from `std::exception`; catching `...` at a top-level boundary is acceptable to log and terminate cleanly.
- Never use **exceptions for normal control flow** (e.g., signalling end-of-collection); they are for exceptional, error conditions only.
- Consider **`std::expected<T, E>`** (C++23) or a library-equivalent for functions where failure is a predictable outcome and performance matters.
- Use **`static_assert`** for compile-time invariants that should never be false; they document assumptions and fail loudly at compile time instead of silently at runtime.
- Use **`assert()`** in debug builds to catch programming mistakes (broken invariants, null preconditions); never `assert()` on user input.
- Provide **meaningful error messages** in exceptions and assertions; the message should explain _what_ was expected vs. _what_ was observed.
- On error paths, ensure all acquired resources are released , RAII handles this automatically; be extra careful in C-interop code.

---

## 8. Templates & Generic Programming

- Use **templates to write type-safe, reusable abstractions** without runtime overhead; prefer templates over `void*` or type erasure when possible.
- **Constrain templates with C++20 concepts** (`requires` clauses or concept-names in place of `typename`) to produce comprehensible error messages and express intent clearly.
- Prefer **`if constexpr`** over specialisation or `std::enable_if` for compile-time branching inside a template body; it is more readable and avoids SFINAE complexity.
- Use **variadic templates** and **fold expressions** rather than recursive template specialisations for parameter packs.
- Avoid **template bloat**: if a template uses a parameter only for a subset of its functionality, factor out the non-generic part into a non-template function.
- Place **template definitions in header files** (or in an included `.tpp` / `.inl` file) because the full definition must be visible to every translation unit that instantiates it.
- Prefer **`auto` return types** and **trailing return types** to improve readability in complex template signatures.
- Use **type traits** (`<type_traits>`) and **concepts** to guard against misuse at compile time rather than at runtime.
- Move **computation from runtime to compile time** with `constexpr` and template metaprogramming wherever it is beneficial and readable.

---

## 9. Concurrency & Parallelism

- **Assume your code will run as part of a multi-threaded program** , avoid global mutable state, prefer const or thread-local data.
- Protect shared mutable state with **`std::mutex`** (or `std::shared_mutex` for read-heavy workloads); never access shared data without synchronisation.
- Prefer **`std::jthread`** (C++20) over `std::thread`; it joins automatically on destruction, preventing the accidental termination caused by a detached or un-joined thread.
- **Never `detach()` a thread** unless you have a clear, safe shutdown mechanism; detached threads holding pointers to local data lead to undefined behaviour.
- Use **`std::atomic<T>`** for single-variable lock-free operations; do not use raw `volatile` for inter-thread communication.
- **Never call unknown code while holding a lock** (e.g., callbacks, virtual functions on external objects) , it can cause deadlocks.
- Minimise the **time spent holding a lock**: acquire late, release early, and avoid blocking I/O inside a critical section.
- Prefer **higher-level concurrency abstractions** (`std::async`, `std::packaged_task`, parallel algorithms via `<execution>`) over raw thread management.
- Use **`std::condition_variable`** with a predicate lambda to avoid spurious wake-ups: `cv.wait(lock, []{ return ready; })`.
- Pass **small, trivially copyable data between threads by value** to avoid data races; use `std::shared_ptr` when threads must share heap-allocated data.
- Consider **lock-free design** or immutable data structures when contention profiling shows mutex overhead is significant.

---

## 10. Performance & Optimization

- **Profile before optimising**: measure with a real profiler (perf, VTune, Instruments) to find the actual bottleneck; premature optimisation is the root of much evil.
- Avoid **"gratuitous pessimisation"**: simple patterns like passing large objects by value, unnecessary copies in loops, or `new`/`delete` in hot paths are correctible without sacrificing readability.
- Prefer **`std::vector`** over other containers for contiguous, cache-friendly memory layout; prefer algorithms that iterate sequentially.
- Access **memory predictably and sequentially**; random access patterns cause cache misses that dominate runtime in modern architectures.
- Prefer **value semantics and local variables** over heap allocation in hot loops; stack memory is always in cache.
- Use **`reserve()`** on vectors and similar containers when you know the approximate size in advance to avoid repeated reallocations.
- Use **move semantics** to avoid deep copies of expensive-to-copy objects like strings, vectors, and maps.
- Rely on the **static type system** for performance: strong types enable compiler optimisations that are impossible with `void*` or type-erased interfaces.
- Use **`constexpr`** and compile-time computation to move work from runtime to compile time wherever correct and readable.
- Prefer **standard library algorithms** (`std::sort`, `std::find_if`, `std::transform`, etc.) over hand-written loops , they are well-optimised and convey intent clearly.
- Enable and use **link-time optimisation (LTO)** and **profile-guided optimisation (PGO)** in release builds for production software.
- Measure the **actual binary size and startup time** for embedded or mobile targets, not just runtime throughput.

---

## 11. Project Structure & Build System (CMake)

- Use **CMake as the build system** for all non-trivial C++ projects; it is the industry standard for cross-platform builds and is understood by all major IDEs.
- Require a **minimum CMake version** at the top of every `CMakeLists.txt`: `cmake_minimum_required(VERSION 3.25)` (or newer).
- Adopt the **modern, target-centric CMake paradigm**: use `target_include_directories()`, `target_compile_options()`, and `target_link_libraries()` with `PRIVATE`/`PUBLIC`/`INTERFACE` scopes instead of the old global `include_directories()` / `add_definitions()`.
- Organise a project as a **tree of CMake targets** (executables, static libraries, shared libraries, interface libraries); each target encapsulates its own dependencies and compile requirements.
- Never hardcode **absolute paths** in CMakeLists.txt; use CMake variables (`${CMAKE_SOURCE_DIR}`, `${PROJECT_SOURCE_DIR}`) and generator expressions.
- Use **`FetchContent`** (CMake ≥ 3.11) or a package manager (vcpkg, Conan) to manage third-party dependencies reproducibly, rather than committing vendored source trees.
- Use **`find_package()`** with version requirements and `REQUIRED` to locate system-installed libraries and fail loudly if they are missing.
- Separate build artefacts from source with **out-of-source builds**: always run CMake from a `build/` directory, never from the source root.
- Define and share **CMake Presets** (`CMakePresets.json`) for common configurations (Debug, Release, RelWithDebInfo, Sanitizers) so all team members use identical flags.
- Enable **`CMAKE_EXPORT_COMPILE_COMMANDS=ON`** to generate `compile_commands.json` for IDE integration and static analysis tools (clang-tidy, clangd).
- Design libraries to be **installable and consumable as packages** using `install()` and CMake's package config files so downstream projects can `find_package()` them.
- Use a **consistent directory layout**: e.g., `src/`, `include/`, `tests/`, `docs/`, `cmake/`, and `third_party/` at the project root.

---

## 12. Testing

- Write **unit tests for every non-trivial function and class**; tests are the executable specification of your design.
- Use an established testing framework: **GoogleTest** (gtest/gmock), Catch2, or doctest , they integrate cleanly with CMake via `FetchContent` and CTest.
- Integrate tests into the build system with **CTest** (`enable_testing()`, `add_test()`) so `cmake --build . && ctest` is the single command to build and test.
- Write tests **before or alongside implementation** (test-driven development) to drive good interface design; testable code is almost always better-structured code.
- Test **edge cases, boundary conditions, and error paths** as thoroughly as the happy path; bugs concentrate at boundaries.
- Keep unit tests **fast and independent**: each test must be able to run in isolation, in any order, without relying on external state (files, network, global singletons).
- Use **mocks and dependency injection** (gmock, hand-rolled fakes) to isolate units under test from their dependencies.
- Measure and enforce **code coverage** with gcov/llvm-cov; aim for high coverage in business-critical code, but do not treat 100% coverage as a goal in itself.
- Run tests with **AddressSanitizer** and **UndefinedBehaviourSanitizer** (`-fsanitize=address,undefined`) to catch memory errors and UB that unit logic does not expose.
- Use **fuzz testing** (libFuzzer, AFL++) for parsers, decoders, and any function that handles untrusted input.
- Treat **test code with the same quality standards as production code**: keep it readable, avoid duplication, refactor it when the API changes.
- Run the full test suite **before every merge** to main; broken main branches are expensive.

---

## 13. Tooling & Static Analysis

- Use **clang-tidy** with a curated set of checks (including `cppcoreguidelines-*`, `modernize-*`, and `readability-*`) to catch issues that compilers do not warn about.
- Enable **clang-format** with a shared `.clang-format` configuration file and enforce it in CI so formatting is never debated in code review.
- Integrate a **static analyser** (clang-tidy, PVS-Studio, Coverity, or SonarQube) into your CI pipeline; treat reported defects with the same urgency as failing tests.
- Run **AddressSanitizer** (`-fsanitize=address`), **LeakSanitizer**, **ThreadSanitizer** (`-fsanitize=thread`), and **UndefinedBehaviourSanitizer** (`-fsanitize=undefined`) in CI on every commit.
- Use **Valgrind** (Linux) or Dr. Memory (Windows) for deeper memory analysis when sanitisers alone are insufficient.
- Use a **debugger actively** (GDB, LLDB, Visual Studio Debugger) rather than adding `std::cout` statements for debugging; step through logic, inspect memory, set watchpoints.
- Configure your **IDE or editor** (CLion, VS Code + clangd, Visual Studio) with LSP/clangd support for real-time error highlighting, code navigation, and refactoring.
- Use **`compile_commands.json`** (generated by CMake) to provide tooling with accurate include paths and compiler flags, avoiding false positives.
- Periodically run **`include-what-you-use` (IWYU)** to remove unnecessary `#include` directives that increase compilation time.
- Use **`ccache`** or **`sccache`** to cache compiled objects, dramatically reducing incremental and CI rebuild times.
- Keep all tooling versions **pinned and reproducible** in CI: use a Docker image, Nix shell, or `.tool-versions` file to prevent "works on my machine" failures.

---

## 14. Deployment & CI/CD

- Use **version control** (Git) for all code, build scripts, CMake files, and configuration; commit frequently after tests pass.
- Never keep files **checked out for long periods**; prefer short-lived feature branches and frequent integration to avoid painful merges.
- Ensure that **checked-in code never breaks the build**: require CI to pass (build + test) before merging any branch into main.
- Implement a **fully automated, one-command build**: `cmake --preset release && cmake --build build/release` should produce a deployable artefact with no manual steps.
- Use **GitHub Actions, GitLab CI, or equivalent** to run build, test, and analysis jobs on every push and pull request across all supported platforms (Linux, macOS, Windows).
- Build and test against **multiple compilers** (GCC, Clang, MSVC) and language standard versions (`-std=c++17`, `-std=c++20`) in CI to catch portability regressions early.
- Use **CPack** (bundled with CMake) to produce platform-native installers (`.deb`, `.rpm`, `.msi`, `.dmg`) or archives from the same build system.
- Tag releases in version control and **produce versioned artefacts** automatically from CI on tag push; never ship builds made on a developer's local machine.
- Keep **debug symbols separate** from release binaries (e.g., `.pdb` on Windows, split DWARF on Linux); store them for post-mortem crash analysis.
- Use **reproducible builds**: pin toolchain versions, library versions, and build flags so the same source always produces the same binary; use vcpkg or Conan with lockfiles.
- Monitor deployed applications with **crash reporters** (Sentry, Breakpad, Crashpad) and collect stack traces to identify and prioritise field defects.
- Apply the **principle of least privilege** in deployment: executables should not run with more operating-system permissions than they need.

---

## 15. Maintenance & Documentation

- **Write code to be read by a human**, not just compiled by a machine; optimise for the next developer who will maintain your code, who may be you in six months.
- Document the **"why"**, not the **"what"**: if code says `x += 2`, a comment that says `// add 2` is noise; a comment that says `// account for the null terminator` is invaluable.
- Use **Doxygen-style comments**Always use Doxygen-style (///) comments for documentation and usual (//) comments for implementation details.
- Generate HTML documentation as part of CI so it stays current.
- Keep the **public API surface as small as possible**: every public symbol is a commitment you must maintain; hide implementation details in anonymous namespaces or `detail::` sub-namespaces.
- Refactor mercilessly: **remove dead code**, extract repeated logic, rename misleading identifiers, and split oversized files when the codebase grows beyond easy comprehension.
- Follow the **Boy Scout Rule**: leave every file you touch slightly cleaner than you found it , fix a warning, add a missing test, or improve a confusing name.
- **Review all code** before merging: more eyes catch more bugs, spread knowledge, and maintain consistent style; automated checks (CI, lint, format) are prerequisites, not substitutes, for human review.
- Use **`[[deprecated("reason")]]`** to mark APIs that should not be used in new code; give a migration path in the deprecation message.
- Track **technical debt explicitly** (issues, TODO comments with ticket numbers) rather than leaving it invisible; allocate time to pay it down regularly.
- Keep **dependencies up to date**: outdated libraries accumulate security vulnerabilities and API drift; use dependabot, Renovate, or regular vcpkg/Conan lockfile updates.
- Maintain a **`CHANGELOG.md`** that records breaking changes, new features, and bug fixes per release; make it easy for users of your library to understand what changed.
- Write a **`CONTRIBUTING.md`** that documents how to build, test, format, and submit changes so that new contributors are productive quickly.

---

_Sources: [LearnCpp.com](https://www.learncpp.com) · [StudyPlan.dev , Intro to C++](https://www.studyplan.dev/intro-to-programming) · [StudyPlan.dev , Pro C++](https://www.studyplan.dev/pro-cpp) · [StudyPlan.dev , CMake](https://www.studyplan.dev/cmake) · [cppreference.com](https://cppreference.com) · [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines) · [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html) · [Herb Sutter & Andrei Alexandrescu , C++ Coding Standards (101 Rules)](https://micro-os-plus.github.io/develop/sutter-101/) · [isocpp.org , Coding Standards FAQ](https://isocpp.org/wiki/faq/coding-standards)_
