# Verdant Siege

C++17 / SFML 2.6 · Top-down wave-defense

\---

### Build (Windows — vcpkg)

```powershell
vcpkg install sfml
cmake .. -DCMAKE\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\_TOOLCHAIN\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\_FILE=$env:VCPKG\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\_ROOT/scripts/buildsystems/vcpkg.cmake
cmake --build .
```

\---

first launch

Install CMake

cd C:\\

git clone https://github.com/microsoft/vcpkg

cd vcpkg

.\\bootstrap-vcpkg.bat

.\\vcpkg install sfml

.\\vcpkg integrate install

cd C:\\VerdantSiege

mkdir build

cd build

cmake ..DCMAKE\_TOOLCHAIN\_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake

cmake --build . --config Release
cd blah blah blah verdantsiege

.\\build\\Release\\VerdantSiege.exe to run

