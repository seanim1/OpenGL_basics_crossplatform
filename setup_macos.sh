#!/usr/bin/env bash
# setup_macos.sh — macOS
set -e

# ── Homebrew ──────────────────────────────────────────────────────────
if ! command -v brew &>/dev/null; then
    echo "Installing Homebrew..."
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
fi

# ── dependencies ──────────────────────────────────────────────────────
brew install cmake sdl2

# ── Dear ImGui ────────────────────────────────────────────────────────
if [ ! -d third_party/imgui ]; then
    echo "Fetching Dear ImGui..."
    mkdir -p third_party
    git clone --depth 1 https://github.com/ocornut/imgui third_party/imgui
fi

# ── CMake configure ───────────────────────────────────────────────────
if [ -d build_xcode ]; then
    echo "build_xcode already exists, skipping configure."
    echo "To reconfigure: rm -rf build_xcode && bash setup_macos.sh"
else
    cmake -S . -B build_xcode -G Xcode
fi

echo ""
echo "Open in Xcode:       open build_xcode/gles2_tutorial.xcodeproj"
echo "Or build terminal:   cmake --build build_xcode --config Release"
echo "Run:                 ./build_xcode/04_imgui/Release/run"
echo ""
echo "To clean:            rm -rf build_xcode"