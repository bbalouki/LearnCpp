# LearnCpp

### From Fundamentals to Professional Ecosystems

_First Edition , by [Bertin Balouki SIMYELI](https://bbalouki.github.io/portfolio/)_

---

## Preface

Welcome to **LEARNCPP**, a hands-on guide to the modern C++ ecosystem.

Most tutorials teach you how to write a `main.cpp` and compile it. That is only the beginning. Today, C++ is used in high-frequency trading, game engines, autonomous vehicles, and systems software , and working in those environments means understanding the full toolchain around the code, not just the code itself.

This project is designed as a working reference. It shows how professional C++ projects are structured and managed , the build system, dependency handling, testing, benchmarking, and tooling. Whether you are a CS student, a self-taught programmer exploring performance, or a developer coming from Python or Java, this repository is meant to give you a concrete, runnable example you can read and learn from.

---

## Table of Contents

**[Chapter 1: The Philosophy of Modern Systems](#chapter-1-the-philosophy-of-modern-systems)**  
**[Chapter 2: Anatomy of a Professional Project](#chapter-2-anatomy-of-a-professional-project)**  
**[Chapter 3: Environment Setup](#chapter-3-environment-setup)**  
**[Chapter 4: VS Code Configuration](#chapter-4-vs-code-configuration)**  
**[Chapter 5: Dependency Management](#chapter-5-dependency-management)**  
**[Chapter 6: The Build Process](#chapter-6-the-build-process)**  
**[Chapter 7: Testing and Mocking](#chapter-7-testing-and-mocking)**  
**[Chapter 8: Performance Benchmarking](#chapter-8-performance-benchmarking)**  
**[Chapter 9: Protocol Buffers](#chapter-9-protocol-buffers)**  
**[Chapter 10: Modern C++ Features (C++20/23)](#chapter-10-modern-c-features-c2023)**  
**[Chapter 11: Low-Level Memory Layout](#chapter-11-low-level-memory-layout)**  
**[Chapter 12: Exercises](#chapter-12-exercises)**  
**[Appendix A: Developer's Glossary](#appendix-a-developers-glossary)**

---

## Chapter 1: The Philosophy of Modern Systems

The goal of this project is to bridge the gap between "writing a single `.cpp` file" and "building a professional, scalable software product." Most tutorials focus heavily on syntax , loops, pointers, classes. That matters, but it is only one piece. Here, we focus on the **entire ecosystem** that makes C++ development work at scale.

### Why This Project Exists

In practice, writing C++ involves much more than invoking a compiler. It requires coordinating a set of tools:

- **Build Orchestration**: How do you manage hundreds of files across Windows, Linux, and macOS without rewriting your configuration every time?
- **Dependency Management**: How do you safely include third-party libraries without breaking your build or introducing version conflicts?
- **Automated Testing**: How do you ensure that a change you make today doesn't silently break something that worked yesterday?
- **Code Quality Automation**: How do you enforce consistent style and catch subtle bugs before they ship?

### What You Will Learn

By working through this repository, you will get hands-on experience with:

1.  **Project Orchestration**: Using **CMake** to manage complex builds across operating systems.
2.  **Modern Build Standards**: Writing with the features of **C++20 and C++23** , safer, cleaner, and more expressive code.
3.  **Dependency Management**: Moving past the "copy-paste header files" approach in favor of package managers like **vcpkg** and **Conan**.
4.  **Test-Driven Mentality**: Structuring testable code with **Interfaces** and **Mocking** via Google Test.
5.  **Performance Analysis**: Measuring code speed objectively with **Google Benchmark** instead of guessing.
6.  **Code Quality Automation**: Using **Clang-Format**, **Clang-Tidy**, and **Static Analysis** to keep the codebase clean.
7.  **Data Serialization**: Integrating **Protocol Buffers (Protobuf)** for structured data exchange , a standard in distributed systems.

---

## Chapter 2: Anatomy of a Professional Project

The directory structure of a C++ project can look confusing at first. Here is a walkthrough of _why_ things are organized the way they are:

- **`include/` (The "What")**: The public headers (`.h` or `.hpp`). These define the contracts of your libraries , what they expose. Consumers of your library only need these files; they do not need to know how anything is implemented.
- **`src/` (The "How")**: The implementation files (`.cpp`). The actual logic lives here, separate from the public interface. This separation of header (declaration) and source (implementation) is a core C++ convention.
- **`tests/`**: Tests live in their own directory so production logic stays independent of test code. Test-specific dependencies never end up in the shipped binary.
- **`benchmarks/`**: Timing tests are kept separate from correctness tests because they serve a different purpose and often need a release build to produce meaningful numbers.
- **`cmake/`**: Custom CMake helper scripts and macros live here, keeping the root-level `CMakeLists.txt` clean and readable.
- **`.vscode/`**: Pre-configured IDE settings so you can focus on learning C++ rather than fighting the editor setup.
- **`examples/`**: Small, self-contained programs showing how to use the libraries. Usually the first place a new contributor will look.

---

## Chapter 3: Environment Setup

Setting up a C++ environment is tricky because the tools vary across operating systems and compilers. Follow the path that matches your setup.

### Path A: Windows via MSYS2 (GCC or Clang)

MSYS2 gives you a Unix-like terminal on Windows and is a solid choice for the MinGW-w64 toolchain.

1.  **Install MSYS2**: Download and run the installer from [msys2.org](https://www.msys2.org/).
2.  **Update package databases**: Open "MSYS2 UCRT64" from the Start menu:
    ```bash
    pacman -Syu
    ```
3.  **Install GCC**:
    ```bash
    pacman -S --needed base-devel mingw-w64-ucrt-x86_64-toolchain
    ```
4.  **Install Clang (optional)**: Clang produces clearer error messages and compiles faster.
    ```bash
    pacman -S --needed base-devel mingw-w64-clang-x86_64-toolchain
    ```
5.  **Install CMake and quality tools**:
    - _GCC/UCRT_: `pacman -S mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-clang-tools-extra mingw-w64-ucrt-x86_64-doxygen mingw-w64-ucrt-x86_64-cppcheck`
    - _Clang_: `pacman -S mingw-w64-clang-x86_64-cmake mingw-w64-clang-x86_64-clang-tools-extra mingw-w64-clang-x86_64-doxygen mingw-w64-clang-x86_64-cppcheck`
6.  **Install vcpkg**:
    - `cd ~`
    - `git clone https://github.com/microsoft/vcpkg.git`
    - `./vcpkg/bootstrap-vcpkg.sh`
      _(Note: The VS Code configs default to `/home/{{username}}/vcpkg` , update this to your actual username.)_

### Path B: Windows via MSVC

Use this path for Windows-only desktop development or environments using Microsoft's MSVC compiler.

1.  **Install Build Tools**: Download [Visual Studio Build Tools 2022](https://visualstudio.microsoft.com/visual-cpp-build-tools/) and select the "Desktop development with C++" workload.
2.  **Install remaining tools with `winget`** (run PowerShell as Administrator):
    ```powershell
    winget install Kitware.CMake
    winget install LLVM.LLVM  # provides clang-format and clang-tidy
    winget install DimitriVanHeesch.Doxygen
    winget install Cppcheck.Cppcheck
    ```
3.  **Install vcpkg**: Use a short path to avoid Windows path-length limits (e.g., `C:\DEV\vcpkg`):
    - `git clone https://github.com/microsoft/vcpkg.git C:\DEV\vcpkg`
    - `.\vcpkg\bootstrap-vcpkg.bat`
4.  **Coverage reporting (optional)**:
    ```powershell
    dotnet tool install --global dotnet-reportgenerator-globaltool
    ```

### Path C: Linux (Ubuntu/Debian)

1.  **Install tools**:
    ```bash
    sudo apt update
    sudo apt install build-essential cmake ninja-build curl git pkg-config cppcheck clang-format clang-tidy doxygen openssh-server fonts-firacode
    ```
2.  **Install vcpkg**:
    - `git clone https://github.com/microsoft/vcpkg.git ~/vcpkg`
    - `~/vcpkg/bootstrap-vcpkg.sh`
    - Add `export VCPKG_ROOT=$HOME/vcpkg` to your `~/.bashrc` or `~/.zshrc`.

### Path D: macOS

1.  **Install the compiler**: `xcode-select --install`
2.  **Install tools via Homebrew** ([brew.sh](https://brew.sh/)):
    ```bash
    brew install cmake ninja doxygen cppcheck llvm
    ```
3.  **Install vcpkg**: Clone to `~/vcpkg`, run the bootstrap script, and add `export VCPKG_ROOT=$HOME/vcpkg` to your `~/.zprofile`.

---

## Chapter 4: VS Code Configuration

### 0. Terminal Setup

Windows C++ development often requires multiple shell environments (PowerShell, Git Bash, MSYS2). VS Code is pre-configured to recognize these via `terminal.integrated.profiles.windows`.

Switch between profiles in the VS Code terminal dropdown. On Windows, **bash (UCRT64)** is recommended as the default for the provided scripts to work correctly.

```json
"terminal.integrated.profiles.windows": {
    "PowerShell": {
      "source": "PowerShell",
      "icon": "terminal-powershell",
    },
    "Git Bash": {
      "source": "Git Bash",
      "icon": "terminal-bash",
    },
    "bash (UCRT64)": {
      "path": "C:\\msys64\\usr\\bin\\bash.exe",
      "icon": "terminal-bash",
      "args": ["--login", "-i"],
      "env": {
        "CHERE_INVOKING": "1",
        "MSYSTEM": "UCRT64",
      },
    },
    "bash (CLANG64)": {
      "path": "C:\\msys64\\usr\\bin\\bash.exe",
      "icon": "terminal-bash",
      "args": ["--login", "-i"],
      "env": {
        "CHERE_INVOKING": "1",
        "MSYSTEM": "CLANG64",
      },
    },
  },
```

VS Code is flexible but needs configuration to work well as a C++ IDE. Four config files in the `.vscode` directory are pre-wired for this project.

### 1. IntelliSense (`c_cpp_properties.json`)

This file tells VS Code where to find your headers. Without it, you will see red squiggles everywhere and "Go to Definition" will not work. Presets for `UCRT64`, `CLANG64`, `Win32`, and `Linux` are included. If you get unexplained editor errors, check that `compilerPath` and `includePath` match where your tools and vcpkg are installed.

### 2. Tasks (`tasks.json`)

Common terminal commands (configure, build, test) are wrapped as VS Code Tasks under `Terminal → Run Task...`. Read the file to see what each task actually runs , they are just `cmake` commands with the boilerplate filled in.

### 3. Debugging (`launch.json`)

Press **F5** and VS Code uses this file to attach a debugger to your binary. It sets the executable path and the debugging engine (`GDB` for Linux/MinGW, `LLDB` for Mac, `cppvsdbg` for MSVC). Set a breakpoint by clicking in the gutter next to a line number, then press F5 , the program will pause there and let you inspect variables.

### 4. Helper Scripts (`scripts/`)

Shell scripts in the `scripts/` directory wrap common workflows for each environment.

- **`scripts/bash.sh`**: A generic Bash script for Linux or Git Bash.
- **`scripts/msys2.sh`**: Optimized for the MSYS2 environment (UCRT64/CLANG64).
- **`scripts/pwh.ps1`**: A PowerShell script designed for the MSVC toolchain on Windows.

#### Key Functions Provided:

- **`coni <profile>`**: A wrapper for `conan install`. It automatically sets up your build folder and handles dependency acquisition with a single command.
- **`build [-p preset] [-b build_type]`**: A high-level command to configure and build your project using CMake presets. It handles the complexity of compiler selection and build type for you.
- **`compile [output_name] [linker_flags]`**: For small experiments or quick tests that don't need a full CMake project, this function directly invokes the compiler with a strict, modern set of C++23 flags.

#### Making It All Work Smoothly:

To make these tools available in every terminal session, you should "source" them in your shell's configuration file:

- **For Bash/MSYS2**: Add `source path/to/scripts/msys2.sh` (or `bash.sh`) to your `~/.bashrc`.
- **For PowerShell**: Add `. path\to\scripts\pwh.ps1` to your PowerShell `$PROFILE`.

Once sourced, you can simply type `build` or `coni default` from any directory within the project, and the scripts will handle the heavy lifting.

---

## Chapter 5: Dependency Management

Historically, integrating a third-party C++ library meant downloading zip files, deciphering the author's build system, compiling for your platform, and manually wiring up linker paths. That process pushed a lot of beginners away from the language.

Package managers like **vcpkg** (and **Conan**) changed this. With a `vcpkg.json` manifest at the root, you declare dependencies instead of managing files manually.

### How vcpkg Works

When CMake runs with the vcpkg toolchain file (pre-wired in the presets), the following happens automatically:

1.  **Download**: vcpkg fetches the correct version of each dependency.
2.  **Compile**: It builds the library from source, matching your compiler and OS settings.
3.  **Wire up**: Include paths and linker flags are injected into CMake automatically , `#include <spdlog/spdlog.h>` just works.

Open `vcpkg.json` at the root of this project to see the dependency list.

### Custom Libraries (`libs/`)

When a library is not in the official vcpkg registry, or you need a modified version, use **Overlay Ports**.

Our custom libraries live in the `libs/` directory (e.g., `libs/imgui/`). Each library requires three essential blueprints:

1.  **`vcpkg.json`**: The manifest file defining the library's name, version, and its own dependencies.
2.  **`portfile.cmake`**: The script that instructs vcpkg how to download (usually from GitHub), configure, and install the library.
3.  **`usage`**: A simple text file that tells future developers exactly how to include this library in their `CMakeLists.txt`.

#### How to Add Your Own Library:

1.  **Create the Directory**: `mkdir libs/your-library-name`
2.  **Define the Manifest**: Create a `vcpkg.json` inside that folder.
3.  **Write the Portfile**: Create `portfile.cmake`. Use the `vcpkg_from_github` function to point to the source code.
    - _Tip_: Set the `SHA512` to `0` initially. Run a build, and vcpkg will fail but provide you with the correct hash to copy-paste.
4.  **Add Usage Instructions**: Create the `usage` file so others know the `find_package` and `target_link_libraries` commands.
5.  **Register the Overlay**: Ensure the `libs` directory is listed in the `overlay-ports` array within `vcpkg-configuration.json` at the project root. (We have already pre-configured this for you!)

By following this pattern, you can vendor any library and have vcpkg manage it like any other dependency.

---

## Chapter 6: The Build Process

This project uses **CMake Presets** (introduced in CMake 3.19) to standardize build configurations across platforms and team members.

### The Two Build Phases

1.  **Configuration**: CMake reads your system, locates the compiler, parses `vcpkg.json`, and generates the actual build files.
    ```bash
    cmake --preset mingw  # substitute 'win' for MSVC, 'cross' for Linux/macOS
    ```
2.  **Building**: The build system compiles your `.cpp` files and links them into an executable.
    ```bash
    cmake --build build
    ```

**A note on Ninja**: Several presets use Ninja as the generator. CMake figures out _what_ to build; Ninja figures out _how to do it as fast as possible_. It is a minimal build executor focused on speed.

---

## Chapter 7: Testing and Mocking

Automated tests ensure that code stays working over time. Without them, a change in one part of the codebase can silently break something else.

### Unit Testing

Look at `tests/greeter/test_greeter.cpp` to see how the `Greeter` library is tested.

**Dependency Injection.**
The `Greeter` class checks the day of the week to say "Happy Monday!" or "Happy Friday!". If we hardcoded a call to the system clock inside the class, we would have to wait for an actual Friday to test that branch. Instead, we pass an abstract `IDateProvider` interface into the Greeter.

### Mocking

Because `Greeter` depends on an interface rather than a concrete clock, we can use **Google Mock** to create a fake `DateProvider` for tests , one that returns whatever day we tell it to. This lets us test every branch of the logic immediately, without waiting for time to pass. It is one of the most useful patterns in testable software design.

---

## Chapter 8: Performance Benchmarking

C++ is frequently chosen for performance, but intuition about what makes code fast is often wrong. The right approach is to measure.

The `benchmarks/bench_main.cpp` file uses **Google Benchmark** to replace guesswork with measurement.

Unlike wrapping a function in a `std::chrono` timer yourself, Google Benchmark is more rigorous: it runs the code thousands or millions of times until it has a statistically stable average. It also provides `benchmark::DoNotOptimize()`, which prevents the compiler from removing code it considers dead , compilers are smart enough to delete a calculation when the result is never used, which would give you a misleading zero-nanosecond result. Google Benchmark works around this to give you honest numbers.

---

## Chapter 9: Protocol Buffers

In distributed systems, different services , possibly written in different languages, running on different machines , need a shared, efficient way to pass data around.

This project includes a working **Protocol Buffers (Protobuf)** example in `src/datamodel/`.

Protobuf lets you define data structures once in a language-agnostic `.proto` file. During the CMake build, a compiler (`protoc`) reads that file and generates C++ classes automatically. Large systems prefer Protobuf over verbose JSON because the binary format is smaller and faster to serialize and parse. This project shows how to integrate Protobuf code generation directly into a standard CMake build.

---

## Chapter 10: Modern C++ Features (C++20/23)

This repository avoids old-style C++ in favor of the C++20 and C++23 standards. Here are the key features you will see used throughout the code:

- **`std::format` (C++20)**: A safe, native way to format strings , replacing the unsafe C-style `printf` and the verbose `std::cout <<` syntax. If you have used Python's f-strings or C#'s string interpolation, this will feel familiar.
- **`std::chrono` (C++20)**: A type-safe library for time and dates. Using raw integers to represent milliseconds is error-prone , unit mismatches are caught at compile time with `std::chrono`. (The $125 million Mars Climate Orbiter was lost in 1999 because two teams used different units. `std::chrono` prevents that class of bug.)
- **`std::unique_ptr`**: In modern C++, you rarely need to write `new` or `delete` directly. Wrapping heap allocations in a `unique_ptr` means the memory is freed automatically when it goes out of scope.
- **`[[maybe_unused]]`**: A compiler attribute that tells the compiler a variable or parameter is intentionally unused. It suppresses the warning without requiring you to comment it out or cast to void.

---

## Chapter 11: Low-Level Memory Layout

High-level abstractions are powerful, but C++'s true strength lies in its ability to manipulate memory directly. To be a master C++ developer, you must understand how data actually sits in RAM.

The `src/memory/` directory contains a series of standalone, educational executables that peel back the layers of abstraction.

### The Memory Laboratory

Unlike the rest of the project which uses a unified build, each file in `src/memory/` is a self-contained lesson:

- **`integers.cpp` & `floats.cpp`**: Visualizing two's complement and IEEE-754 representation.
- **`characters.cpp`**: Understanding ASCII, UTF-8, and how strings are just byte arrays.
- **`images.cpp`**: How pixels are packed into rows and the memory cost of different color depths.
- **`sounds.cpp`**: Representing waveforms as PCM samples.
- **`videos.cpp` & `video_sound.cpp`**: The complexity of multiplexing audio/video streams into containers.

### Interactive Documentation

For a visual guided tour, open `docs/memory-layout.html` in any web browser.

This interactive dashboard acts as a live textbook for the `src/memory/` source files. It explains the "why" behind the bit-shifting and pointer arithmetic you see in the code, providing a bridge between theoretical computer science and practical systems programming.

---

## Chapter 12: Exercises

The best way to learn from this project is to change things and see what happens. Once your environment is set up, try the following:

### Exercise 1: Change the greeting

1.  Open `src/greeter/Greeter.cpp`.
2.  Modify the string returned by `simple_greet()` , for example, change it to `"Welcome to the Developer Lab!"`.
3.  Rebuild and run the binary. Does your new message appear?

### Exercise 2: Fix the failing test

1.  The change from Exercise 1 will break an existing test. Run: `ctest --test-dir build`
2.  The test will fail , that is expected. It shows the safety net is working.
3.  Open `tests/greeter/test_greeter.cpp` and update the expected string to match your new message.
4.  Run CTest again. It should pass.

### Exercise 3: Add a new day

1.  Look at the `switch` statement in `Greeter.cpp`.
2.  Add a case for Wednesday (day 3), returning something like `"Happy Hump Day!"`.
3.  Add a corresponding test in `test_greeter.cpp` using `MockDateProvider` to force the day to Wednesday and `ASSERT_EQ` to verify the new string.

---

## Appendix A: Developer's Glossary

- **Compiler**: Software that translates human-readable C++ into binary machine instructions (e.g., GCC, Clang, MSVC).
- **Build System Generator**: A tool that reads configuration files and coordinates compiling and linking files in the correct order (e.g., CMake).
- **Toolchain**: The full set of tools , compiler, linker, archiver, debugger , needed to produce a program for a specific architecture.
- **Header File (`.h` / `.hpp`)**: Declares _what_ entities exist , interfaces, class definitions, function signatures.
- **Source File (`.cpp`)**: Contains the actual implementation of what the header declares.
- **Linker**: Takes compiled object files (`.o` / `.obj`) and links them together, resolving references between files to produce the final executable.
- **Static Analysis**: Tools that scan source code for bugs and security flaws without running the program (e.g., Clang-Tidy).
- **Sanitizers**: Compiler instrumentation (like AddressSanitizer) that monitors the program at runtime and reports memory errors or data races the moment they occur.

---

## Epilogue

C++ has a steep learning curve, and the ecosystem is broad. That is exactly why this project exists , to give you a concrete, working example to read, run, and modify.

You will hit cryptic linker errors. Read the output carefully; the messages tell you what went wrong. Over time, they become easier to interpret.

The lab is open. The tools are yours.

**Happy coding.**
