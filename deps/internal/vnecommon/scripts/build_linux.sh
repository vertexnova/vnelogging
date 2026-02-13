#!/bin/bash
# ----------------------------------------------------------------------
# Copyright (c) 2024 Ajeet Singh Yadav. All rights reserved.
# Licensed under the Apache License, Version 2.0 (the "License")
#
# Author:    Ajeet Singh Yadav
# Created:   January 2025
# ----------------------------------------------------------------------

set -e

# Default values
BUILD_TYPE="Debug"
BUILD_EXAMPLES="ON"
CLEAN_BUILD=0
COMPILER=""

# Parse arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --release)
            BUILD_TYPE="Release"
            shift
            ;;
        --debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        --no-examples)
            BUILD_EXAMPLES="OFF"
            shift
            ;;
        --clean)
            CLEAN_BUILD=1
            shift
            ;;
        --gcc)
            COMPILER="gcc"
            shift
            ;;
        --clang)
            COMPILER="clang"
            shift
            ;;
        --help)
            echo "Usage: $0 [options]"
            echo ""
            echo "Options:"
            echo "  --release      Build in Release mode"
            echo "  --debug        Build in Debug mode (default)"
            echo "  --no-examples  Don't build examples"
            echo "  --clean        Clean build directory before building"
            echo "  --gcc          Use GCC compiler"
            echo "  --clang        Use Clang compiler"
            echo "  --help         Show this help message"
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

# Get script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_DIR/build/$BUILD_TYPE"

echo "=== VneCommon Build Script ==="
echo "Build Type: $BUILD_TYPE"
echo "Build Examples: $BUILD_EXAMPLES"
echo "Build Directory: $BUILD_DIR"

# Set compiler if specified
CMAKE_COMPILER_ARGS=""
if [ "$COMPILER" = "gcc" ]; then
    CMAKE_COMPILER_ARGS="-DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++"
    echo "Compiler: GCC"
elif [ "$COMPILER" = "clang" ]; then
    CMAKE_COMPILER_ARGS="-DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++"
    echo "Compiler: Clang"
fi
echo ""

# Clean build if requested
if [ $CLEAN_BUILD -eq 1 ]; then
    echo "Cleaning build directory..."
    rm -rf "$BUILD_DIR"
fi

# Create build directory
mkdir -p "$BUILD_DIR"

# Configure
echo "Configuring..."
cmake -B "$BUILD_DIR" -S "$PROJECT_DIR" \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    -DBUILD_EXAMPLES="$BUILD_EXAMPLES" \
    $CMAKE_COMPILER_ARGS

# Build
echo "Building..."
cmake --build "$BUILD_DIR" --config "$BUILD_TYPE" --parallel

echo ""
echo "=== Build Complete ==="
echo "Output: $BUILD_DIR/bin/"
