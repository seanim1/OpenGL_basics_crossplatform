#!/usr/bin/env bash
# setup.sh — Linux / Raspberry Pi
set -e

sudo apt-get update
sudo apt-get install -y \
    cmake \
    libgles2-mesa-dev libegl-dev \
    libsdl2-dev pkg-config

if [ ! -d third_party/imgui ]; then
    echo "Fetching Dear ImGui..."
    mkdir -p third_party
    git clone --depth 1 https://github.com/ocornut/imgui third_party/imgui
fi

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

echo ""
echo "Build all:        cmake --build build"
echo "Build a target:   cmake --build build --target 04_imgui"
echo "Run:              ./build/04_imgui/04_imgui"