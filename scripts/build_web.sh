#!/bin/bash

#==============================================================================
# VneLogging Web Build Script
#==============================================================================
# Copyright (c) 2024 Ajeet Singh Yadav. All rights reserved.
# Licensed under the Apache License, Version 2.0 (the "License")
#
# Author:    Ajeet Singh Yadav
# Created:   January 2025
#
# This script builds VneLogging for the Web platform using Emscripten
#==============================================================================

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Function to show usage
show_usage() {
    echo "Usage: $0 [OPTIONS] [BUILD_TYPE]"
    echo ""
    echo "Options:"
    echo "  -c, --clean        Clean build directory before building"
    echo "  -f, --force        Force rebuild (clean + build)"
    echo "  -j, --jobs N       Number of parallel jobs (default: auto-detect)"
    echo "  -h, --help         Show this help message"
    echo ""
    echo "Build types:"
    echo "  Debug              Debug build (default)"
    echo "  Release            Release build"
    echo "  RelWithDebInfo     Release with debug info"
    echo "  MinSizeRel         Minimum size release"
    echo ""
    echo "Examples:"
    echo "  $0                    # Debug build"
    echo "  $0 Release            # Release build"
    echo "  $0 -c Release         # Clean Release build"
    echo "  $0 -f -j4 Debug       # Force rebuild with 4 jobs"
}

# Function to get number of CPU cores cross-platform
get_cpu_cores() {
    if [[ "$OSTYPE" == "darwin"* ]]; then
        sysctl -n hw.ncpu
    elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
        nproc
    elif [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "cygwin" ]]; then
        nproc
    else
        echo 4
    fi
}

# Parse command line arguments
BUILD_TYPE="Debug"
CLEAN_BUILD=false
FORCE_REBUILD=false
JOBS=$(get_cpu_cores)

while [[ $# -gt 0 ]]; do
    case $1 in
        -c|--clean)
            CLEAN_BUILD=true
            shift
            ;;
        -f|--force)
            FORCE_REBUILD=true
            CLEAN_BUILD=true
            shift
            ;;
        -j|--jobs)
            if [[ -n "$2" && "$2" =~ ^[0-9]+$ ]]; then
                JOBS="$2"
                shift 2
            else
                print_error "Invalid number of jobs: $2"
                exit 1
            fi
            ;;
        -j*)
            JOBS="${1#-j}"
            if [[ ! "$JOBS" =~ ^[0-9]+$ ]]; then
                print_error "Invalid number of jobs: $JOBS"
                exit 1
            fi
            shift
            ;;
        -h|--help)
            show_usage
            exit 0
            ;;
        Debug|Release|RelWithDebInfo|MinSizeRel)
            BUILD_TYPE="$1"
            shift
            ;;
        *)
            print_error "Unknown option: $1"
            show_usage
            exit 1
            ;;
    esac
done

# Get script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

print_status "VneLogging Web Build Script"
print_status "Project root: $PROJECT_ROOT"
print_status "Build type: $BUILD_TYPE"
print_status "Parallel jobs: $JOBS"
print_status "Clean build: $CLEAN_BUILD"
print_status "Force rebuild: $FORCE_REBUILD"

# Check if we're in the right directory
if [[ ! -f "$PROJECT_ROOT/CMakeLists.txt" ]]; then
    print_error "CMakeLists.txt not found. Please run this script from the project root."
    exit 1
fi

# Check if Emscripten is available
# Try to find Emscripten in common locations
EMSDK_DIR=""
if [[ -d "$PROJECT_ROOT/3rd_party/emsdk" ]]; then
    EMSDK_DIR="$PROJECT_ROOT/3rd_party/emsdk"
elif [[ -d "$HOME/emsdk" ]]; then
    EMSDK_DIR="$HOME/emsdk"
elif [[ -n "$EMSDK" ]]; then
    EMSDK_DIR="$EMSDK"
else
    print_error "Emscripten SDK not found"
    print_error "Please install Emscripten SDK:"
    print_error "  git clone https://github.com/emscripten-core/emsdk.git ~/emsdk"
    print_error "  cd ~/emsdk && ./emsdk install latest && ./emsdk activate latest"
    exit 1
fi

# Setup Emscripten environment
print_status "Setting up Emscripten environment..."

# Platform-specific Emscripten environment setup
if [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "cygwin" ]] || [[ "$OSTYPE" == "win32" ]] || [[ "$OSTYPE" == "msys2" ]]; then
    # Windows: Use PowerShell-compatible environment setup
    if [[ -f "$EMSDK_DIR/emsdk_env.ps1" ]]; then
        print_status "Using PowerShell Emscripten environment"
        export EMSDK="$EMSDK_DIR"
        export PATH="$EMSDK_DIR:$PATH"
        if [[ -d "$EMSDK_DIR/upstream/emscripten" ]]; then
            export PATH="$EMSDK_DIR/upstream/emscripten:$PATH"
        fi
    else
        print_error "PowerShell Emscripten environment script not found"
        exit 1
    fi
else
    # Unix/Linux/macOS: Use bash environment setup
    if [[ -f "$EMSDK_DIR/emsdk_env.sh" ]]; then
        source "$EMSDK_DIR/emsdk_env.sh"
    else
        print_error "Emscripten environment script not found"
        exit 1
    fi
fi

# Check if Emscripten is installed
if ! command -v emcc &> /dev/null; then
    print_warning "Emscripten not installed. Installing latest version..."
    cd "$EMSDK_DIR"

    # Platform-specific Emscripten installation
    if [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "cygwin" ]] || [[ "$OSTYPE" == "win32" ]] || [[ "$OSTYPE" == "msys2" ]]; then
        if [[ -f "./emsdk.ps1" ]]; then
            powershell -ExecutionPolicy Bypass -File ./emsdk.ps1 install latest
            powershell -ExecutionPolicy Bypass -File ./emsdk.ps1 activate latest
        else
            print_error "PowerShell Emscripten SDK script not found"
            exit 1
        fi
    else
        ./emsdk install latest
        ./emsdk activate latest
        source ./emsdk_env.sh
    fi

    cd "$PROJECT_ROOT"

    # Re-source environment after installation
    if [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "cygwin" ]] || [[ "$OSTYPE" == "win32" ]] || [[ "$OSTYPE" == "msys2" ]]; then
        export EMSDK="$EMSDK_DIR"
        export PATH="$EMSDK_DIR:$PATH"
        if [[ -d "$EMSDK_DIR/upstream/emscripten" ]]; then
            export PATH="$EMSDK_DIR/upstream/emscripten:$PATH"
        fi
    else
        source "$EMSDK_DIR/emsdk_env.sh"
    fi
fi

# Check Emscripten version
EMCC_VERSION=$(emcc --version | head -n1)
print_status "Using Emscripten: $EMCC_VERSION"

# Verify Emscripten environment is properly set
if ! command -v emcc &> /dev/null; then
    print_error "Emscripten compiler (emcc) not found in PATH"
    print_error "Emscripten environment not properly set up"
    exit 1
fi

# Extract version number for build directory
EMCC_VERSION_NUM=$(echo "$EMCC_VERSION" | grep -o '[0-9]\+\.[0-9]\+\.[0-9]\+' | head -n1)
if [[ -z "$EMCC_VERSION_NUM" ]]; then
    EMCC_VERSION_NUM="unknown"
fi

# Create build directory with consistent format
BUILD_DIR="$PROJECT_ROOT/build/${BUILD_TYPE}/build-web-emscripten-${EMCC_VERSION_NUM}"
print_status "Build directory: $BUILD_DIR"

# Handle clean build
if [[ "$CLEAN_BUILD" == true ]]; then
    if [[ -d "$BUILD_DIR" ]]; then
        print_status "Cleaning build directory..."
        rm -rf "$BUILD_DIR"
        print_success "Build directory cleaned"
    else
        print_warning "Build directory does not exist, nothing to clean"
    fi
fi

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Check if CMake cache exists and handle force rebuild
if [[ "$FORCE_REBUILD" == true ]] || [[ ! -f "CMakeCache.txt" ]] || ! grep -q "EMSCRIPTEN:INTERNAL=1" CMakeCache.txt 2>/dev/null; then
    print_status "Configuring CMake for Emscripten..."

    # Clean any existing configuration that might be using wrong toolchain
    if [[ -f "CMakeCache.txt" ]]; then
        print_status "Removing existing CMake cache to ensure proper Emscripten toolchain"
        rm -f CMakeCache.txt
    fi

    cmake "$PROJECT_ROOT" \
        -G "Unix Makefiles" \
        -DCMAKE_TOOLCHAIN_FILE="$EMSDK_DIR/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake" \
        -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
        -DBUILD_TESTS=ON

    # Verify Emscripten toolchain is being used
    if grep -q "EMSCRIPTEN:INTERNAL=1" CMakeCache.txt 2>/dev/null || grep -q "EMSCRIPTEN:BOOL=TRUE" CMakeCache.txt 2>/dev/null; then
        print_success "Emscripten toolchain confirmed"
    else
        print_error "Emscripten toolchain not detected in CMakeCache.txt"
        print_error "Please check that Emscripten is properly installed and configured"
        exit 1
    fi
else
    print_status "Using existing Emscripten CMake configuration"
fi

# Build the project
print_status "Building VneLogging for Web..."

# Platform-specific build command
if [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "cygwin" ]] || [[ "$OSTYPE" == "win32" ]] || [[ "$OSTYPE" == "msys2" ]]; then
    if grep -q "CMAKE_GENERATOR:INTERNAL=Unix Makefiles" CMakeCache.txt 2>/dev/null; then
        print_status "Detected Unix Makefiles generator, using make"
        if command -v make &> /dev/null; then
            make -j"$JOBS"
        else
            print_error "make command not found. Please install make for Windows or use a different generator."
            exit 1
        fi
    else
        print_status "Detected Windows platform, using CMake build system"
        cmake --build . --config $BUILD_TYPE --parallel $JOBS
    fi
else
    print_status "Detected Unix/Linux/macOS platform, using make"
    make -j"$JOBS"
fi

print_success "Build completed successfully!"
print_status "Output files are in: $BUILD_DIR"
