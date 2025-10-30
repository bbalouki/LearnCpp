## VCPKG
vcpkg new --application
vcpkg x-update-baseline --add-initial-baseline

## CONAN
conan profile detect --name gcc-debug # create new profile
conan install . --output-folder=build --build=missing --profile gcc-debug
conan lock create conanfile.txt

## PROTOBUF
protoc --proto_path=. --cpp_out=. filename.proto
pip install mypy-protobuf # For type hints
protoc --proto_path=. --python_out=. --mypy_out=. filename.proto

## COVERAGE ANALYSIS
    # MYSYS2
    pacman -S mingw-w64-ucrt-x86_64-python-lxml # using UCRT64
    pacman -S mingw-w64-clang-x86_64-python-lxml # using CLANG64
    python -m venv --system-site-packages .venv
    source .venv/bin/activate
    pip install gcovr

cmake -S . -B build/ -DCMAKE_BUILD_TYPE=Debug -DCODE_COVERAGE=ON
cmake --preset {preset} --build build/
cmake --build build/
ctest --test-dir build/
cmake --build build/ --target coverage
gcovr -r . --filter 'src/' --html 'build/coverage.html'

## VCPKG PACKAGING
https://learn.microsoft.com/en-us/vcpkg/get_started/get-started-packaging?pivots=shell-bash
mkdir packaging
cd packaging && mkdir {{packag}} && touch vcpkg.json portfile.cmake usage
vcpkg install --overlay-ports=./packaging {{packag}}
update SHA512 value in the portfile.cmake
vcpkg install --overlay-ports=./packaging {{packag}}

Fork the vcpkg Repository
Add your fork as a remote
git remote add myfork https://github.com/<Your-GitHub-Username>/vcpkg.git
git remote -v
git checkout -b {{package}}
cp -R path/to/{{package}} ports/{{package}}
git add ports/{{package}}
git commit -m "Add {{ package}} library to vcpkg"

vcpkg format-manifest "path/to/{{package}}/vcpkg.json"
vcpkg x-add-version itch
git s
git commit -m "version database"

git push myfork {{package}}
