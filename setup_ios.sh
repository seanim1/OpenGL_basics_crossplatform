#!/usr/bin/env bash
# setup_ios.sh — iOS arm64
# For real device builds set your Team ID first:
#   export APPLE_TEAM_ID=XXXXXXXXXX
#   Find it at: https://developer.apple.com/account -> Membership
#   Or just open Xcode and sign with your free Apple ID
set -e

DEPLOYMENT_TARGET="15.0"
TEAM_ID="${APPLE_TEAM_ID:-}"

# ── Xcode check ───────────────────────────────────────────────────────
if ! command -v xcodebuild &>/dev/null; then
    echo "error: Xcode not found. Install from the App Store."
    exit 1
fi

xcodebuild -version

# ── CMake ─────────────────────────────────────────────────────────────
brew install cmake

# ── SDL2 for iOS device (built from source) ───────────────────────────
if [ ! -d third_party/SDL2 ]; then
    echo "Fetching SDL2..."
    mkdir -p third_party
    git clone --depth 1 --branch release-2.32.x \
        https://github.com/libsdl-org/SDL third_party/SDL2
else
    echo "SDL2 source already present, skipping."
fi

if [ ! -d third_party/SDL2/build_ios ]; then
    echo "Building SDL2 for iOS device..."
    cmake -S third_party/SDL2 -B third_party/SDL2/build_ios -G Xcode \
        -DCMAKE_SYSTEM_NAME=iOS \
        -DCMAKE_OSX_ARCHITECTURES=arm64 \
        -DCMAKE_OSX_DEPLOYMENT_TARGET=${DEPLOYMENT_TARGET} \
        -DCMAKE_INSTALL_PREFIX="$(pwd)/third_party/SDL2/install_ios" \
        -DSDL_SHARED=OFF \
        -DSDL_STATIC=ON \
        -DSDL_TEST=OFF
    cmake --build third_party/SDL2/build_ios --config Release -- \
        CODE_SIGNING_ALLOWED=NO
    cmake --install third_party/SDL2/build_ios --config Release
    echo "SDL2 for iOS device built and installed."
else
    echo "SDL2 for iOS device already built, skipping."
fi

# ── Dear ImGui ────────────────────────────────────────────────────────
if [ ! -d third_party/imgui ]; then
    echo "Fetching Dear ImGui..."
    mkdir -p third_party
    git clone --depth 1 https://github.com/ocornut/imgui third_party/imgui
else
    echo "Dear ImGui already present, skipping."
fi

# ── GLM ───────────────────────────────────────────────────────────────
if [ ! -d third_party/glm ]; then
    echo "Fetching GLM..."
    mkdir -p third_party
    git clone --depth 1 https://github.com/g-truc/glm third_party/glm
else
    echo "GLM already present, skipping."
fi

# ── CMake configure ───────────────────────────────────────────────────
SDL2_DIR="$(pwd)/third_party/SDL2/install_ios/lib/cmake/SDL2"

if [ -z "$TEAM_ID" ]; then
    echo ""
    echo "WARNING: APPLE_TEAM_ID not set."
    echo "You can set signing in Xcode manually with your free Apple ID."
    echo ""
fi

if [ -d build_ios ]; then
    echo "build_ios already exists, skipping configure."
    echo "To reconfigure: rm -rf build_ios && ./setup_ios.sh"
else
    cmake -S . -B build_ios -G Xcode \
        -DCMAKE_SYSTEM_NAME=iOS \
        -DCMAKE_OSX_ARCHITECTURES=arm64 \
        -DCMAKE_OSX_DEPLOYMENT_TARGET=${DEPLOYMENT_TARGET} \
        -DSDL2_DIR=${SDL2_DIR} \
        -DCMAKE_PREFIX_PATH=${SDL2_DIR} \
        -DCMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM="${TEAM_ID}" \
        -DCMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY="iPhone Developer" \
        -DCMAKE_XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY="1,2"
fi

echo ""
echo "Next steps:"
echo ""
echo "1. Open in Xcode:"
echo "   open build_ios/gles2_tutorial.xcodeproj"
echo ""
echo "2. For each target (01_hello_triangle_SDL2, 02_triangle_color_attribute,"
echo "   03_transform, 04_imgui):"
echo "   - Click the target in the left sidebar"
echo "   - Signing & Capabilities -> Automatically manage signing"
echo "   - Set Team to your Apple ID (free account works)"
echo ""
echo "3. Real device:"
echo "   - Connect your iPhone"
echo "   - Trust this Mac on your iPhone when prompted"
echo "   - Select your iPhone from the device dropdown"
echo "   - Select a target scheme (e.g. 04_imgui)"
echo "   - Press Run"
echo ""
echo "To clean: rm -rf build_ios third_party/SDL2"