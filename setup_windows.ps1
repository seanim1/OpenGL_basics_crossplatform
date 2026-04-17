# setup_windows.ps1
# Run from: x64 Native Tools Command Prompt for VS 2022
# Usage: powershell -ExecutionPolicy Bypass -File setup_windows.ps1
$ErrorActionPreference = "Stop"

# ── vcpkg ─────────────────────────────────────────────────────────────
if (-not (Test-Path "C:\vcpkg")) {
    Write-Host "Cloning vcpkg..."
    git clone https://github.com/microsoft/vcpkg C:\vcpkg
    & C:\vcpkg\bootstrap-vcpkg.bat
}

# ── vcpkg packages ────────────────────────────────────────────────────
$packages = @("sdl2:x64-windows", "angle:x64-windows")
foreach ($pkg in $packages) {
    $name = $pkg.Split(":")[0]
    $installed = & C:\vcpkg\vcpkg.exe list | Select-String $name
    if (-not $installed) {
        Write-Host "Installing $pkg..."
        & C:\vcpkg\vcpkg.exe install $pkg
    } else {
        Write-Host "$pkg already installed, skipping."
    }
}

# ── Dear ImGui ────────────────────────────────────────────────────────
if (-not (Test-Path "third_party\imgui")) {
    Write-Host "Fetching Dear ImGui..."
    New-Item -ItemType Directory -Force -Path third_party | Out-Null
    git clone --depth 1 https://github.com/ocornut/imgui third_party\imgui
} else {
    Write-Host "Dear ImGui already present, skipping."
}

# ── CMake configure ───────────────────────────────────────────────────
$toolchain = "C:\vcpkg\scripts\buildsystems\vcpkg.cmake"

if (Test-Path "build_windows") {
    Write-Host "build_windows exists, skipping cmake configure."
    Write-Host "To reconfigure: Remove-Item -Recurse -Force build_windows"
} else {
    cmake -S . -B build_windows "-DCMAKE_TOOLCHAIN_FILE=$toolchain"
}

# ── build ─────────────────────────────────────────────────────────────
cmake --build build_windows --config Release

# ── copy libEGL.dll next to every SDL2 exe ────────────────────────────
# libEGL.dll is a runtime dependency of SDL2 on Windows — SDL2 loads it
# dynamically so vcpkg does not copy it automatically for SDL2 targets.
$egl = "C:\vcpkg\installed\x64-windows\bin\libEGL.dll"
$dirs = @(
    "build_windows\01_hello_triangle_SDL2\Release",
    "build_windows\02_triangle_color_attribute\Release",
    "build_windows\03_transform\Release",
    "build_windows\04_imgui\Release"
)
foreach ($dir in $dirs) {
    if (Test-Path $dir) {
        Copy-Item $egl $dir -Force
        Write-Host "Copied libEGL.dll to $dir"
    }
}

Write-Host ""
Write-Host "Open in Visual Studio:   start build_windows\gles2_tutorial.sln"
Write-Host "Or build from terminal:  cmake --build build_windows --config Release"
Write-Host "Run:                     .\build_windows\04_imgui\Release\04_imgui.exe"
Write-Host "Run:                     .\build_windows\01_hello_triangle_SDL2\Release\01_hello_triangle_SDL2.exe"
Write-Host "Run:                     .\build_windows\02_triangle_color_attribute\Release\02_triangle_color_attribute.exe"
Write-Host "Run:                     .\build_windows\03_transform\Release\03_transform.exe"
