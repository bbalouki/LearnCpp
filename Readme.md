For MSYS2:
Use MSYS2(UCRT64) terminal in Vscode to run commands.
To install packages, follow these steps:
    1 search for the package you want to install using `pacman -Ss <package_name>`.
    2 install the package using `pacman -S <package_name>`.
    3 If the package is not available, search in [vcpkg](https://vcpkg.io/en/packages?query=) and install it using `vcpkg install <package_name>`. 
    4 if all packages are installed, then run cmake+vcpkage in classic mode, else run cmake+vcpkg in manifest mode.

For the lunch.json file, set the externalConsole to true to use the MSYS2(UCRT64) terminal and set stopAtEntry to true for debugging.
Set x64-mingw-static as the default triplet in vcpkg for static linking.

In manifest mode, if you using MSYS2, make sure to call vcpkg new --application using the mingw installed vcpkg
if using MSVC use vcpkg new --application with the windows installed vcpkg.


