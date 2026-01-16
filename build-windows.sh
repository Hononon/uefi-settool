#!/bin/bash
# Cross-compile script for Windows using MXE

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$SCRIPT_DIR/build-windows"

# MXE path - adjust if installed elsewhere
MXE_PATH="${MXE_PATH:-$HOME/mxe}"

if [ ! -d "$MXE_PATH/usr/bin" ]; then
    echo "Error: MXE not found at $MXE_PATH"
    echo "Set MXE_PATH environment variable to your MXE installation"
    exit 1
fi

export PATH="$MXE_PATH/usr/bin:$PATH"

CMAKE_CMD="x86_64-w64-mingw32.static-cmake"
if ! command -v "$CMAKE_CMD" &> /dev/null; then
    echo "Error: $CMAKE_CMD not found in MXE"
    exit 1
fi

echo "=== Cross-compiling UEFI Boot Order Manager (Windows x64) ==="
echo "Using MXE: $MXE_PATH"

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

$CMAKE_CMD .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

echo "=== Build complete ==="
echo "Binary: $BUILD_DIR/uefi-settool.exe"
