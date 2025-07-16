@echo off
TITLE Vector Slicer installation

SET mypath=%~dp0
set pwd=%mypath:~0,-1%
IF EXIST "C:\src\vcpkg\" (
    cd C:\src\vcpkg\
    git pull
) else (
     mkdir C:\src
     cd C:\src
     git clone https://github.com/Microsoft/vcpkg.git
     cd vcpkg && bootstrap-vcpkg.bat
)

vcpkg install boost:x64-windows
vcpkg integrate install

set cmake_path="C:/src/vcpkg/scripts/buildsystems/vcpkg.cmake"

echo Installing Vector Slicer
cd %pwd%

IF EXIST ".git" (
    git pull
) else (
    echo Warning: Git directory does not exist, which will prevent you from easily updating the project in the future.
    echo Please consider cloning the GitHub repository, by going into the desired parent directory and running:
    echo git clone https://github.com/Zmmyslony/vector_slicer.git
)

cmake -S ./ -B ./build -DCMAKE_TOOLCHAIN_FILE=%cmake_path%
cmake --build ./build --config Release -j4

setx VECTOR_SLICER_OUTPUT "%pwd%\output"
setx VECTOR_SLICER_API "%pwd%\build\Release\vector_slicer_api.dll"

cmd /k