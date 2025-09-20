## VCPKG
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
    pacman -S mingw-w64-clang-x86_64-python-lxml
    python -m venv --system-site-packages .venv
    source .venv/bin/activate
    pip install gcovr

gcovr -r . --filter 'src/' --html 'build/coverage.html'
