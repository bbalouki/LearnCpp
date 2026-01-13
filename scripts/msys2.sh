PS1='\[\033[01;32m\]\u@\h\[\033[00m\]:\[\033[01;34m\] \W\[\033[00m\]\n\$ '
alias python="python3"
alias rmf="rm -rf"

# -----------------------------------------------------------------------------
# Function: compile
# Description:
#   Directly compiles all C++ source files found in the current directory and
#   its subdirectories into a single executable.
#
#   This function is a simple, self-contained build script for small projects
#   that do not use a formal build system like CMake. It is specifically
#   designed to work within an MSYS2 environment (e.g., Git Bash on Windows)
#   and automatically selects the compiler (g++ or clang++) based on the
#   active subsystem ($MSYSTEM variable).
#
# Usage:
#   compile [output_name] [linker_flags...]
#
# Arguments:
#   output_name    (Optional) The name for the final executable.
#                  Defaults to "Program". The .exe extension is handled
#                  automatically and should not be included.
#
#   linker_flags... (Optional) Any additional flags to pass to the linker,
#                   such as library links (e.g., -lsfml-graphics).
#
# Key Behavior:
#   - Cleans the 'build' directory completely before starting.
#   - Auto-detects the compiler based on the MSYS2 environment:
#     - UCRT64:  Uses g++.exe
#     - CLANG64: Uses clang++.exe
#   - Compiles with a strict set of modern C++23 flags.
#   - Places the final executable inside the 'build' directory.
#
# Examples:
#   # Compile all .cpp files into "build/Program.exe"
#   compile
#
#   # Compile into "build/my_app.exe"
#   compile my_app
#
#   # Compile and link against the Raylib library
#   compile game -lraylib -lopengl32 -lgdi32 -lwinmm
# -----------------------------------------------------------------------------
function compile() {
    echo "MSYSTEM=$MSYSTEM"
    rm -rf build

    local -a SOURCES
    while IFS= read -r -d '' file; do
        SOURCES+=("$file")
    done < <(find . -name "*.cpp" -print0)

    if [ ${#SOURCES[@]} -eq 0 ]; then
        echo "Error: No .cpp files found." >&2
        return 1
    fi

    # Pick compiler based on environment
    local COMPILER
    local DEBUG_FLAG
    case "$MSYSTEM" in
        UCRT64)
            COMPILER="/ucrt64/bin/g++.exe"
            DEBUG_FLAG="-ggdb"
            ;;
        CLANG64)
            COMPILER="/clang64/bin/clang++.exe"
            DEBUG_FLAG="-g"
            ;;
        *) echo "Unsupported MSYSTEM=$MSYSTEM"; return 1 ;;
    esac

    local output_file="Program"
    if [[ -n "$1" && "$1" != -* ]]; then
        output_file="${1%.exe}"
        shift
    fi
    local -a LINKER_FLAGS=("$@")

    local -a CXX_FLAGS=(
        -std=c++23
        "$DEBUG_FLAG"
        -Werror
        -Wshadow
        -Wall
        -Wextra
        -Wconversion
        -Wsign-conversion
        -pedantic-errors
        -Wpedantic
    )

    mkdir -p build
    local OUTPUT_FILE="build/${output_file}"

    echo "--- Compiling with $COMPILER "
    if "$COMPILER" "${CXX_FLAGS[@]}" "${SOURCES[@]}" "${LINKER_FLAGS[@]}" -o "$OUTPUT_FILE"; then
        echo "Compilation successful!"
        echo "Executable: ${OUTPUT_FILE}.exe"
    else
        echo "Compilation failed!" >&2
        return 1
    fi
}


# -----------------------------------------------------------------------------
# Function: build
# Description:
#   Configures and builds a CMake project using presets.
#
#   This function is a convenient wrapper around the standard two-step CMake
#   process. It is designed for an MSYS2 environment and automatically selects
#   the appropriate 'cmake.exe' and C++ compiler based on the active shell.
#
# Prerequisites:
#   - A 'CMakeLists.txt' file must exist in the project's root directory.
#   - A 'CMakePresets.json' file should exist to define the presets.
#
# Usage:
#   build [-p|--preset PRESET_NAME] [-b|--build-type BUILD_TYPE]
#
# Options:
#   -p, --preset PRESET_NAME
#       Specifies the CMake preset to use for configuration.
#       Defaults to "default".
#
#   -b, --build-type BUILD_TYPE
#       Specifies the build type (e.g., Debug, Release, RelWithDebInfo).
#       Defaults to "Debug".
#
# Key Behavior:
#   - Auto-detects cmake and the C++ compiler for the MSYS2 environment.
#   - Passes the chosen compiler and build type directly to CMake, ensuring
#     a predictable and explicit configuration.
#
# Examples:
#   # Configure and build using the "default" preset and "Debug" build type
#   build
#
#   # Build using the "release" preset
#   build -p release
#
#   # Build using the "default" preset and "Release" build type
#   build -b Release
#
#   # Build using a custom preset with the Release build type
#   build --preset my_release_config --build-type Release
# -----------------------------------------------------------------------------
function build() {
    local preset="default"
    local build_type="Debug"

    # Parse options
    while [[ $# -gt 0 ]]; do
        case $1 in
            -p|--preset)
                preset="$2"
                shift 2
                ;;
            -b|--build-type)
                build_type="$2"
                shift 2
                ;;
            *)
                echo "Unknown option: $1"
                echo "Usage: build [-p preset] [-b build_type]"
                return 1
                ;;
        esac
    done

    # Pick cmake path and compiler based on environment
    local CMAKE_BIN
    local CXX_COMPILER

    case "$MSYSTEM" in
        UCRT64)
            CMAKE_BIN="/ucrt64/bin/cmake.exe"
            CXX_COMPILER="/ucrt64/bin/g++.exe"
            ;;
        CLANG64)
            CMAKE_BIN="/clang64/bin/cmake.exe"
            CXX_COMPILER="/clang64/bin/clang++.exe"
            ;;
        *)
            echo "Unsupported MSYSTEM=$MSYSTEM"
            return 1
            ;;
    esac
    echo "MSYSTEM=$MSYSTEM"
    echo "Configuring with preset='$preset', build_type='$build_type', compiler=$CXX_COMPILER..."

    # Configure with explicit compiler selection
    "$CMAKE_BIN" --preset="$preset" \
        -DCMAKE_BUILD_TYPE="$build_type" \
        -DCMAKE_CXX_COMPILER="$CXX_COMPILER" || return 1

    # Build
    "$CMAKE_BIN" --build build
}

case "$MSYSTEM" in
  UCRT64)
    export CC="/ucrt64/bin/gcc"
    export CXX="/ucrt64/bin/g++"
    ;;
  CLANG64)
    export CC="/clang64/bin/clang"
    export CXX="/clang64/bin/clang++"
    ;;
esac

export VCPKG_ROOT="/home/Bertin/vcpkg"
export VCPKG_DEFAULT_TRIPLET="x64-mingw-static"
export VCPKG_DEFAULT_HOST_TRIPLET="x64-mingw-static"
export PATH="$PATH:/home/Bertin/vcpkg"
