#!/usr/bin/env bash
# setup_ios.sh — iOS (arm64, simulator + device)
# Requires: macOS with Xcode installed
# Apple Developer account required for real device builds
set -e

DEPLOYMENT_TARGET="15.0"
TEAM_ID="${APPLE_TEAM_ID:-}"  # set via: export APPLE_TEAM_ID=XXXXXXXXXX

# ── Xcode check ───────────────────────────────────────────────────────
if ! command -v xcodebuild &>/dev/null; then
    echo "error: Xcode not found. Install from the App Store."
    exit 1
fi

xcodebuild -version

# ── Homebrew + CMake ──────────────────────────────────────────────────
if ! command -v brew &>/dev/null; then
    echo "Installing Homebrew..."
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
fi

brew install cmake

# ── SDL2 for iOS (built from source) ─────────────────────────────────
# Homebrew SDL2 is macOS only — we need to build SDL2 targeting iOS
if [ ! -d third_party/SDL2 ]; then
    echo "Fetching SDL2..."
    mkdir -p third_party
    git clone --depth 1 https://github.com/libsdl-org/SDL third_party/SDL2
fi

if [ ! -d third_party/SDL2/build_ios ]; then
    echo "Building SDL2 for iOS..."
    cmake -S third_party/SDL2 -B third_party/SDL2/build_ios -G Xcode \
        -DCMAKE_SYSTEM_NAME=iOS \
        -DCMAKE_OSX_ARCHITECTURES=arm64 \
        -DCMAKE_OSX_DEPLOYMENT_TARGET=${DEPLOYMENT_TARGET} \
        -DSDL_SHARED=OFF \
        -DSDL_STATIC=ON
    cmake --build third_party/SDL2/build_ios --config Release
    echo "SDL2 for iOS built."
else
    echo "SDL2 for iOS already built, skipping."
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
if [ -z "$TEAM_ID" ]; then
    echo ""
    echo "WARNING: APPLE_TEAM_ID not set."
    echo "Simulator builds will work but real device builds will fail."
    echo "Set it with: export APPLE_TEAM_ID=XXXXXXXXXX"
    echo "Find your Team ID at: https://developer.apple.com/account"
    echo ""
fi

SDL2_IOS_DIR="$(pwd)/third_party/SDL2/build_ios"

if [ -d build_ios ]; then
    echo "build_ios already exists, skipping configure."
    echo "To reconfigure: rm -rf build_ios && bash setup_ios.sh"
else
    cmake -S . -B build_ios -G Xcode \
        -DCMAKE_SYSTEM_NAME=iOS \
        -DCMAKE_OSX_ARCHITECTURES=arm64 \
        -DCMAKE_OSX_DEPLOYMENT_TARGET=${DEPLOYMENT_TARGET} \
        -DSDL2_DIR=${SDL2_IOS_DIR} \
        -DCMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM="${TEAM_ID}" \
        -DCMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY="iPhone Developer" \
        -DCMAKE_XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY="1,2"  # 1=iPhone 2=iPad
fi

echo ""
echo "Open in Xcode:"
echo "  open build_ios/gles2_tutorial.xcodeproj"
echo ""
echo "Build + run on simulator (no account needed):"
echo "  xcodebuild -project build_ios/gles2_tutorial.xcodeproj \\"
echo "    -scheme run \\"
echo "    -destination 'platform=iOS Simulator,name=iPhone 15' \\"
echo "    -configuration Release build"
echo ""
echo "Build for real device (needs APPLE_TEAM_ID):"
echo "  xcodebuild -project build_ios/gles2_tutorial.xcodeproj \\"
echo "    -scheme run \\"
echo "    -destination 'generic/platform=iOS' \\"
echo "    -configuration Release build"
echo ""
echo "To clean: rm -rf build_ios third_party/SDL2/build_ios"