#!/usr/bin/env bash
# setup.sh
set -e

sudo apt-get install -y \
    meson ninja-build \
    libgles2-mesa-dev libegl-dev \
    libsdl2-dev

# vendor Dear ImGui if not present
if [ ! -d third_party/imgui ]; then
    echo "Fetching Dear ImGui..."
    mkdir -p third_party
    git clone --depth 1 https://github.com/ocornut/imgui third_party/imgui
fi

meson setup builddir

echo ""
echo "Build all:        ninja -C builddir"
echo "Build 04 only:    ninja -C builddir 04_imgui/run"
echo "Run:              ./builddir/04_imgui/run"
