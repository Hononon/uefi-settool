#!/bin/bash
# Build script for Linux native build

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$SCRIPT_DIR/build-linux"

echo "=== Building UEFI Boot Order Manager (Linux) ==="

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

echo "=== Build complete ==="
echo "Binary: $BUILD_DIR/uefi-settool"
