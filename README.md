# OpenGL ES 2.0 & SDL2/ImGui tutorials
### Tested: Linux✅, Windows✅, MacOS, Android, iOS

### Linux
- Open terminal
  ```bash
  bashrm -rf build
  cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
  cmake --build build

### Windows
- Download MSVC cpp build tools if you haven't [MSVC Compiler](https://visualstudio.microsoft.com/visual-cpp-build-tools/)
- In the Windows search bar, look for "x64 Native Tools Command Prompt for VS 2022"
- Verify compiler exists:
  ```Windows Command Prompt
    cl
- Build:
  ```Windows Command Prompt
  rmdir /s /q build_windows
  powershell -ExecutionPolicy Bypass -File setup_windows.ps1
  cmake --build build_windows --config Release
- Run:
  ```Windows Command Prompt
  .\build_windows\00_probe_OpenGLES2\Release\00_probe_OpenGLES2.exe
  .\build_windows\01_hello_triangle_SDL2\Release\01_hello_triangle_SDL2.exe
  .\build_windows\04_imgui\Release\04_imgui.exe
