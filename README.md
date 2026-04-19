# OpenGL ES 2.0 & SDL2/ImGui tutorials
### Tested: Linux✅, Windows✅, MacOS, Android, iOS

### Linux
- Run once for initialization
  ```bash
  ./setup.sh
- Run every time a file is modified
  - Build:
    - Build all:
      ```bash
      cmake --build build
    - Targeted build:
      ```bash
      cmake --build build --target 00_probe_OpenGLES2
      cmake --build build --target 01_hello_triangle_SDL2
      cmake --build build --target 04_imgui
  - Execute program:
    ```bash
    ./build/00_probe_OpenGLES2/00_probe_OpenGLES2
    ./build/01_hello_triangle_SDL2/01_hello_triangle_SDL2
    ./build/04_imgui/04_imgui
- Clear
  ```bash
  rm -rf build
### Windows
- Download MSVC cpp build tools if you haven't [MSVC Compiler](https://visualstudio.microsoft.com/visual-cpp-build-tools/)
- In the Windows search bar, look for "x64 Native Tools Command Prompt for VS 2022"
- Verify compiler exists:
  ```Windows Command Prompt
    cl
- Run once for initialization
  ```bash
  powershell -ExecutionPolicy Bypass -File setup.ps1
- Run every time a file is modified
  - Build:
    - Build all:
      ```bash
      cmake --build build_windows --config Release
    - Targeted build:
      ```bash
      cmake --build build_windows --target 00_probe_OpenGLES2 --config Release
      cmake --build build_windows --target 01_hello_triangle --config Release
      cmake --build build_windows --target 04_imgui --config Release
  - libEGL.dll is missing for some reason, copy them in to directories with .exe
    ```bash
    powershell -ExecutionPolicy Bypass -File patch_dll.ps1
  - Execute program:
    ```bash
    build_windows\00_probe_OpenGLES2\Release\00_probe_OpenGLES2.exe
    build_windows\01_hello_triangle_SDL2\Release\01_hello_triangle_SDL2.exe
    build_windows\04_imgui\Release\04_imgui.exe
- Clear
  ```bash
  rmdir /s /q build_windows
### MacOS
