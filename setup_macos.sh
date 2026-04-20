#!/usr/bin/env bash
# setup_macos.sh — macOS Apple Silicon (M1+)
set -e

# ── Homebrew deps ─────────────────────────────────────────────────────
brew install cmake sdl2 pkgconf

# ── vcpkg ─────────────────────────────────────────────────────────────
if [ ! -d ~/vcpkg ]; then
    echo "Cloning vcpkg..."
    git clone https://github.com/microsoft/vcpkg ~/vcpkg
    ~/vcpkg/bootstrap-vcpkg.sh
fi

# ── ANGLE via vcpkg ───────────────────────────────────────────────────
ANGLE_INSTALLED=$(~/vcpkg/vcpkg list | grep "^angle" || true)
if [ -z "$ANGLE_INSTALLED" ]; then
    echo "Installing ANGLE..."
    ~/vcpkg/vcpkg install angle:arm64-osx
else
    echo "ANGLE already installed, skipping."
fi

# ── Dear ImGui ────────────────────────────────────────────────────────
if [ ! -d third_party/imgui ]; then
    echo "Fetching Dear ImGui..."
    mkdir -p third_party
    git clone --depth 1 https://github.com/ocornut/imgui third_party/imgui
else
    echo "Dear ImGui already present, skipping."
fi

# ── CMake configure ───────────────────────────────────────────────────
TOOLCHAIN=~/vcpkg/scripts/buildsystems/vcpkg.cmake

if [ -d build_macos ]; then
    echo "build_macos already exists, skipping configure."
    echo "To reconfigure: rm -rf build_macos && bash setup_macos.sh"
else
    cmake -S . -B build_macos -G Xcode \
        -DCMAKE_OSX_ARCHITECTURES=arm64 \
        -DCMAKE_OSX_DEPLOYMENT_TARGET=12.0 \
        -DVCPKG_TARGET_TRIPLET=arm64-osx \
        -DCMAKE_TOOLCHAIN_FILE=$TOOLCHAIN
fi

echo ""
echo "Open in Xcode:       open build_macos/gles2_tutorial.xcodeproj"
echo "Or build terminal:   cmake --build build_macos --config Release"
echo "Run:                 ./build_macos/04_imgui/Release/04_imgui"
echo ""
echo "To clean:            rm -rf build_macos"