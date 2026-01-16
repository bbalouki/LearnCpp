# Function: coni
# Description: A wrapper for 'conan install' that simplifies the command syntax.
#              It installs dependencies into a local 'build' folder and 
#              builds any missing binaries from source.
# Prerequisites:
#    - conan must be installed on your system and add on the system Path
#       or installed via python and accessible in the current session.
#
# Usage: coni <profile_name>
#
# Arguments:
#   $1 (profile_name): The specific Conan profile to use (e.g., default, debug).
#
# Returns:
#   0: If installation succeeds.
#   1: If the profile name argument is missing or conan fails.
coni() {
    if [ -z "$1" ]; then
        echo "Error: Please provide a profile name."
        echo "Usage: coni <profile_name>" 
        return 1
    fi
    conan install . --output-folder=build --build=missing --profile "$1"
}


# Build the project using CMake presets
# Usage: build [-c|--compiler COMPILER] [-b|--build-type BUILD_TYPE] [preset_name]
#   COMPILER: g++ (default) or clang++
#   BUILD_TYPE: Debug (default), Release, RelWithDebInfo, or MinSizeRel
#   preset_name: Optional name of the CMake preset to use (default: "default")
function build() {
    local preset="default"
    local cxx_compiler="g++"
    local build_type="Debug" 

    while [[ $# -gt 0 ]]; do
        case "$1" in
            -c|--compiler)
                if [ -z "$2" ]; then
                    echo "Error: --compiler option requires an argument (g++ or clang++)." >&2
                    return 1
                fi
                if [[ "$2" != "g++" && "$2" != "clang++" ]]; then
                    echo "Error: Invalid compiler '$2'. Use 'g++' or 'clang++'." >&2
                    return 1
                fi
                cxx_compiler="$2"
                shift 2
                ;;
            -b|--build-type)
                if [ -z "$2" ]; then
                    echo "Error: --build-type option requires an argument." >&2
                    return 1
                fi
                case "$2" in
                    Debug|Release|RelWithDebInfo|MinSizeRel)
                        build_type="$2"
                        shift 2
                        ;;
                    *)
                        echo "Error: Invalid build type '$2'." >&2
                        echo "Use one of: Debug, Release, RelWithDebInfo, MinSizeRel" >&2
                        return 1
                        ;;
                esac
                ;;
            *)
                preset="$1"
                shift 
                ;;
        esac
    done

    local compiler_path
    compiler_path=$(command -v "$cxx_compiler")
    if [ -z "$compiler_path" ]; then
        echo "Error: Compiler '$cxx_compiler' not found in PATH." >&2
        return 1
    fi

    echo "Using preset: $preset"
    echo "Using build type: $build_type"
    echo "Using C++ compiler: $compiler_path"

    cmake --preset="$preset" \
          -D CMAKE_CXX_COMPILER="$compiler_path" \
          -D CMAKE_BUILD_TYPE="$build_type"
    
    local configure_status=$?
    if [ $configure_status -ne 0 ]; then
        echo "CMake configuration failed!" >&2
        return $configure_status
    fi

    cmake --build --preset "$preset"

    local build_status=$?
    if [ $build_status -eq 0 ]; then
        echo "Build successful!"
    else
        echo "Build failed!"
        return $build_status
    fi
}

# Compile c++ code
# Usage: compile [-c|--compiler COMPILER] [output_name] [linker_flags...]
#   COMPILER: g++ (default) or clang++
#   output_name: Optional name for the executable (default: "Program")
#   linker_flags: Any additional flags to pass to the linker (e.g., -lsfml-graphics)
function compile() {
    local compiler="g++"
    local output_file="Program"

    local linker_flags=() 

    while [[ $# -gt 0 ]]; do
        case "$1" in
            -c|--compiler)
                if [ -z "$2" ]; then
                    echo "Error: --compiler option requires an argument (g++ or clang++)." >&2
                    return 1
                fi
                if [[ "$2" != "g++" && "$2" != "clang++" ]]; then
                    echo "Error: Invalid compiler '$2'. Use 'g++' or 'clang++'." >&2
                    return 1
                fi
                compiler="$2"
                shift 2
                ;;
            -*)
                linker_flags+=("$1")
                shift
                ;;
            *)
                output_file="$1"
                shift
                linker_flags+=("$@")
                break
                ;;
        esac
    done

    local sources
    mapfile -d '' sources < <(find . -name "*.cpp" -print0)
    if [ ${#sources[@]} -eq 0 ]; then
        echo "No .cpp files found!"
        return 1
    fi

    echo "Using compiler: $compiler"
    local cxx_flags=(
        -std=c++23
        -ggdb
        -Werror
        -Wshadow
        -Wall
        -Wextra
        -Wconversion
        -Wsign-conversion
        -pedantic-errors
        -Wpedantic
    )
    "$compiler" "${cxx_flags[@]}" "${sources[@]}" "${linker_flags[@]}" -o "$output_file"

    if [ $? -eq 0 ] && [ -f "$output_file" ]; then
        echo "Compilation successful!"
        echo "Program: $output_file"
    else
        echo "Compilation failed!"
        return 1
    fi
}
