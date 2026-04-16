#!/usr/bin/env bash
# setup.sh — install deps and configure
set -e

# install system deps
sudo apt-get install -y \
    meson ninja-build \
    libgles2-mesa-dev libegl-dev \
    libsdl2-dev

# configure once into a build dir
meson setup builddir

echo ""
echo "Build everything:          ninja -C builddir"

echo "Build targeting probe:          ninja -C builddir 00_probe_OpenGLES2/probe"
echo "Run probe:                 ./builddir/00_probe_OpenGLES2/probe"

echo "Build targeting triangle:          ninja -C builddir 01_hello_triangle_SDL2/triangle"
echo "Run triangle:              ./builddir/01_hello_triangle_SDL2/triangle"
