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
