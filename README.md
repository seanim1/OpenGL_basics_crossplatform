# OpenGL_basics_crossplatformHow to clear and rebuild:
Linux:
bashrm -rf build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

Windows (PowerShell):
rmdir /s /q build_windows
powershell -ExecutionPolicy Bypass -File setup_windows.ps1
cmake --build build_windows --config Release

Then find in .\build_windows\{99_PROJECT}\Release\{99_PROJECT}.exe

macOS (after opening CMake GUI and pointing it at the repo):
bashrm -rf build_xcode
cmake -S . -B build_xcode -G Xcode
open build_xcode/gles2_tutorial.xcodeproj
