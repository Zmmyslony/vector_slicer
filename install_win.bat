@echo off
TITLE Vector Slicer installation

SET cwd=$(pwd)
IF EXIST "C:\src\vcpkg\" (
    cd C:\src\vcpkg\
    git pull
    vcpkg install boost:x64-windows
    vcpkg integrate install
) else (
     mkdir C:\src
     cd C:\src
     git clone https://github.com/Microsoft/vcpkg.git
     cd vcpkg && bootstrap-vcpkg.bat
     vcpkg install boost:x64-windows
     vcpkg integrate install
)

cmake_path=-DCMAKE_TOOLCHAIN_FILE=C:\src\vcpkg\scripts\buildsystems\vcpkg.cmake

@echo Installing Vector Slicer
cd cwd
git pull
cmake -S ./ -B ./build $(cmake_path)
cmake --build ./build --config Release -j4

