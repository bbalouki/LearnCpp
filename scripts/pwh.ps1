
$Env:VCPKG_ROOT = "C:\DEV\vcpkg"
$env:PATH = "C:\Program Files\CMake\bin;" + $Env:VCPKG_ROOT + ";" + $env:PATH

function rmf { 
    param([string[]]$Path)
    Remove-Item -Path $Path -Recurse -Force
}

# -----------------------------------------------------------------------------
# Variable: $vcvarsPath
# .DESCRIPTION
#   A global variable holding the hardcoded path to the MSVC environment
#   setup script (vcvars64.bat).
#
#   NOTE This path may need to be adjusted based on your specific Visual
#   Studio version (e.g., 2019, 2022) and edition (e.g., Community,
#   Professional, BuildTools).
# -----------------------------------------------------------------------------
$vcvarsPath = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat'

# -----------------------------------------------------------------------------
# Function: lvcvars
# .DESCRIPTION
#   Loads the Microsoft Visual C++ (MSVC) developer environment into the
#   current PowerShell session.
#
#   This function executes the 'vcvars64.bat' script provided by Visual
#   Studio. This script sets up essential environment variables (like PATH,
#   INCLUDE, and LIB) required to run the MSVC compiler (cl.exe), linker,
#   and other build tools directly from the command line.
#
# Prerequisites:
#   - Visual Studio or Visual Studio Build Tools must be installed.
#   - The global variable '$vcvarsPath' must point to the correct location
#     of the 'vcvars64.bat' script on your system.
#
# .EXAMPLE
#   lvcvars
#
# Key Behavior:
#   - Checks if the script at '$vcvarsPath' exists.
#   - Runs the script in a temporary 'cmd.exe' shell and captures the
#     environment variables it sets.
#   - Parses the captured variables and applies them to the current
#     PowerShell session's environment.
#   - Provides clear error messages if the script is not found or fails.
#
# .EXAMPLE
#   # Load the MSVC environment into the current shell
#   lvcvars
#
#   # After loading, you can use MSVC tools directly
#   cl.exe /?
#   compile my_app.cpp # (Assumes the 'compile' function is also loaded)
# -----------------------------------------------------------------------------
function lvcvars {
    if (Test-Path $vcvarsPath) {
        # Capture environment from vcvars64.bat via CMD and apply to the current Powershell session
        $output = cmd /c "`"$vcvarsPath`" ^>nul && set" 2>$null
        if ($LASTEXITCODE -eq 0 -and $output){
            $output | ForEach-Object {
                if ($_ -match "^(.*?)=(.*)$") {
                    Set-Item -Path Env:$($matches[1]) -Value $matches[2]
                }
            }
        } else {
            Write-Host "Error: Failed to load MSVC Developer environment." -ForegroundColor Red
        }
    }else {
        Write-Host "Error: MSVC setup script not found at path:`n$vcvarsPath" -ForegroundColor Red
    }
}

<#
.SYNOPSIS
    A wrapper for 'conan install' to streamline dependency installation.

.DESCRIPTION
    The Invoke-ConanInstall function runs the Conan package manager installation 
    command for the current directory. It enforces specific build settings:
    1. Outputs generated files to a local 'build' directory.
    2. Builds missing binary packages from source automatically.
    3. Uses the specified Conan profile.

Prerequisites:
    - conan must be installed on your system and add on the system Path
        or installed via python and accessible in the current session.

.PARAMETER ProfileName
    The name of the Conan profile to use (e.g., 'default', 'debug', 'release').
    This parameter is mandatory.

.EXAMPLE
    Invoke-ConanInstall -ProfileName default
    
    Installs dependencies using the 'default' profile.

.EXAMPLE
    Invoke-ConanInstall debug

    Installs dependencies using the 'debug' profile (positional argument).

.LINK
    https://docs.conan.io/
#>
function Invoke-ConanInstall {
    param (
        [Parameter(Mandatory=$true, Position=0)]
        [string]$ProfileName
    )

    conan install . --output-folder=build --build=missing --profile $ProfileName
}
Set-Alias coni Invoke-ConanInstall


# -----------------------------------------------------------------------------
# Function: build
# .DESCRIPTION
#   Configures and builds a CMake project using PowerShell.
#
#   This function simplifies the standard two-step CMake process by providing
#   simple command-line options for presets and build types. It is designed
#   for a Windows environment and automatically searches for the `cmake.exe`
#   executable in common locations (e.g., Program Files, MSYS2) and uses
#   the first one it finds.
#
# Prerequisites:
#   - A 'CMakeLists.txt' file must exist in the project's root directory.
#   - A 'CMakePresets.json' file should exist to define the presets.
#
# .EXAMPLE
#   build [-Preset <string>] [-BuildType <string>]
#   build [-p <string>] [-b <string>]
#
# Parameters:
#   -Preset, -p <string>
#       Specifies the CMake preset to use for configuration.
#       Defaults to "default".
#
#   -BuildType, -b <string>
#       Specifies the build type (e.g., Debug, Release, RelWithDebInfo).
#       The '--config' flag is used, making it suitable for multi-config
#       generators like Visual Studio. Defaults to "Debug".
#
# .EXAMPLE
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
#   build -Preset my_config -BuildType Release
# -----------------------------------------------------------------------------
function build {
    param (
        [Alias("p")]
        [string]$Preset = "default",

        [Alias("b")]
        [string]$BuildType = "Debug"
    )

    # Define possible cmake executable paths on Windows
    $cmakePaths = @(
        "C:\Program Files\CMake\bin\cmake.exe",
        "C:\msys64\ucrt64\bin\cmake.exe"
    )

    # Find the first available cmake executable
    $cmake = $cmakePaths | Where-Object { Test-Path $_ } | Select-Object -First 1

    if (-not $cmake) {
        Write-Host "Error: CMake executable not found in the specified Windows paths." -ForegroundColor Red
        return
    }

    Write-Host "Using CMake at: $cmake" -ForegroundColor Blue
    Write-Host "Configuring project with preset: $Preset and build type: $BuildType" -ForegroundColor Blue

    # Run cmake configuration step
    & $cmake "--preset=$Preset"
    if ($LASTEXITCODE -eq 0) {
        Write-Host "Configuration successful. Starting build for $BuildType..." -ForegroundColor Blue
        & $cmake --build build --config $BuildType
        if ($LASTEXITCODE -eq 0) {
            Write-Host "Build completed successfully." -ForegroundColor Green
        } else {
            Write-Host "Build failed." -ForegroundColor Red
        }
    } else {
        Write-Host "CMake configuration failed." -ForegroundColor Red
    }
}


# -----------------------------------------------------------------------------
# Function: compile
# .DESCRIPTION
#   Directly compiles all C++ source files in the current directory and its
#   subdirectories using the Microsoft Visual C++ (MSVC) compiler (cl.exe).
#
#   This function is a simple script for small projects that do not use a
#   build system like CMake. It provides a quick way to compile an executable
#   with a standard set of debug flags.
#
# Prerequisites:
#   This function MUST be run from a shell where the Visual Studio environment
#   has been initialized, such as a "Developer Command Prompt for VS" or after
#   running the vcvarsall.bat script. Otherwise, `cl.exe` will not be found.
#
# .EXAMPLE
#   compile [output_name] [linker_flags...]
#
# Arguments:
#   output_name    (Optional) The name for the final executable.
#                  Defaults to "Program.exe". The .exe extension is added
#                  automatically if omitted.
#
#   linker_flags... (Optional) Any additional arguments to pass to the linker,
#                   such as library files (e.g., user32.lib).
#
# Key Behavior:
#   - Cleans the 'build' directory completely before starting.
#   - Finds all .cpp files recursively.
#   - Hardcodes the output directory to 'build\Debug'.
#   - Compiles with MSVC-specific flags for debugging (/Zi), C++ standard
#     (/std:c++latest), and strict warnings (/W4 /WX).
#   - Cleans up generated program database (.pdb) files from the root folder.
#
# .EXAMPLE
#   # Compile all .cpp files into "build\Debug\Program.exe"
#   compile
#
#   # Compile into "build\Debug\my_app.exe"
#   compile my_app
#
#   # Compile and link against the user32.lib and gdi32.lib Windows libraries
#   compile my_win_app user32.lib gdi32.lib
# -----------------------------------------------------------------------------
function compile {
    $sources = ls -Recurse -Filter *.cpp

    if ($sources.Count -eq 0) {
        Write-Host "No .cpp files found!"
        return
    }
    $sourcePaths = $sources.FullName -join " "
    if (Test-Path "build") {
        rm -Recurse -Force "build"
    }
    $OutputFile = "Program.exe"
    $OutputDir = "build\Debug"
    New-Item -ItemType Directory -Force -Path $OutputDir | Out-Null

    if ($args.Count -gt 0) {
        $OutputFile = $args[0]
        $args = $args[1..$args.Length]
    }

    if (-not $OutputFile.ToLower().EndsWith(".exe")) {
        $OutputFile += ".exe"
    }

    $FullOutputPath = Join-Path $OutputDir $OutputFile
    $LinkerFlags = $args -join " "

    $clArguments = @(
        "/nologo",
        "/std:c++latest",
        "/Zi",              # Generate complete debugging information
        "/EHsc",            # Specifies the C++ exception handling model
        "/Od",              # Disables optimization (for debugging)
        "/MDd",             # Links with the debug multithreaded CRT DLL
        "/W4",              # Sets warning level to 4 (high)
        "/WX",              # Treats all warnings as errors
        "/permissive-",     # Sets standard-conformance mode
        "/options:strict",
        $sourcePaths,
        $LinkerFlags,
        "/Fo:`"$OutputDir`"\",
        "/Fe:`"$FullOutputPath`""
    )
    $clCommand = "cl " + ($clArguments -join " ")
    Invoke-Expression $clCommand

    if ($? -and (Test-Path $FullOutputPath) -and ($FullOutputPath.ToLower().EndsWith(".exe"))) {
        Write-Host "Compilation successful!" -ForegroundColor Green
        Write-Host "Program: $FullOutputPath"

    } else {
        Write-Host "Compilation failed!" -ForegroundColor Red
    }
    # Clean up PDB files from the project root
    Remove-Item "*.pdb" -ErrorAction SilentlyContinue
}
