#!/bin/bash

#==============================================================================
# VneLogging Windows Build Script
#==============================================================================
# Copyright (c) 2024 Ajeet Singh Yadav. All rights reserved.
# Licensed under the Apache License, Version 2.0 (the "License")
#
# Author:    Ajeet Singh Yadav
# Created:   January 2025
#
# This script builds VneLogging for Windows with Visual Studio integration
#==============================================================================

set -e  # Exit on any error

# Default parallel jobs
JOBS=10

# Parse command line arguments for jobs
while [[ $# -gt 0 ]]; do
    case $1 in
        -j|--jobs)
            if [[ -n "$2" && "$2" =~ ^[0-9]+$ ]]; then
                JOBS="$2"
                shift 2
            else
                echo "Invalid number of jobs: $2"
                exit 1
            fi
            ;;
        -j*)
            JOBS="${1#-j}"
            if [[ ! "$JOBS" =~ ^[0-9]+$ ]]; then
                echo "Invalid number of jobs: $JOBS"
                exit 1
            fi
            shift
            ;;
        *)
            ARGS+=("$1")
            shift
            ;;
    esac
done

# Reconstruct ARGS for the rest of the script
set -- "${ARGS[@]}"

# Platform detection (force Windows)
PLATFORM="Windows"
COMPILER="cl"

# Function to display usage information
usage() {
  echo "Usage: $0 [-t <build_type>] [-a <action>] [-clean] [-interactive] [-j <jobs>]"
  echo "Options:"
  echo "  -t <build_type>    Specify the build type (Debug, Release, RelWithDebInfo, MinSizeRel)"
  echo "  -a <action>        Specify the action to perform:"
  echo "                     configure: Configure the project"
  echo "                     build: Build the project"
  echo "                     configure_and_build: Configure and build the project"
  echo "                     test: Run tests"
  echo "  -clean             Clean the build directory before performing the action"
  echo "  -interactive       Run in interactive mode to choose options"
  echo "  -j <jobs>          Number of parallel jobs (default: 10)"
  echo ""
  echo "Examples:"
  echo "  $0 -t Debug -a configure_and_build"
  echo "  $0 -j 8"
  echo "  $0 -interactive"
  echo "  $0 -t Release -clean"
  exit 1
}

# Function to run interactive mode
interactive_mode() {
  echo "=== VneLogging Interactive Build Configuration ==="
  echo ""
  echo "Detected Platform: $PLATFORM"
  echo ""

  # Build type selection
  echo "Select Build Type:"
  echo "1) Debug (default) - Development with debug symbols"
  echo "2) Release - Optimized for production"
  echo "3) RelWithDebInfo - Release with debug info"
  echo "4) MinSizeRel - Minimum size release"
  read -p "Enter choice (1-4) [1]: " build_choice
  case $build_choice in
    2) BUILD_TYPE="Release" ;;
    3) BUILD_TYPE="RelWithDebInfo" ;;
    4) BUILD_TYPE="MinSizeRel" ;;
    *) BUILD_TYPE="Debug" ;;
  esac

  # Action selection
  echo ""
  echo "Select Action:"
  echo "1) Configure only"
  echo "2) Configure and build (default)"
  echo "3) Configure, build, and test"
  read -p "Enter choice (1-3) [2]: " action_choice
  case $action_choice in
    1) ACTION="configure" ;;
    3) ACTION="test" ;;
    *) ACTION="configure_and_build" ;;
  esac

  # Clean build option
  echo ""
  read -p "Clean build directory before starting? (y/N): " clean_choice
  if [[ $clean_choice =~ ^[Yy]$ ]]; then
    CLEAN_BUILD=true
  fi

  echo ""
  echo "=== Configuration Summary ==="
  echo "Platform: $PLATFORM"
  echo "Build Type: $BUILD_TYPE"
  echo "Compiler: $COMPILER"
  echo "Action: $ACTION"
  echo "Clean Build: $CLEAN_BUILD"
  echo "Parallel Jobs: $JOBS"
  echo ""
  read -p "Proceed with this configuration? (Y/n): " proceed
  if [[ $proceed =~ ^[Nn]$ ]]; then
    echo "Build cancelled."
    exit 0
  fi
}

# Default values
BUILD_TYPE="Debug"
ACTION="configure_and_build"
COMPILER="cl"
CLEAN_BUILD=false
INTERACTIVE_MODE=false

# Parse command-line arguments
while [[ $# -gt 0 ]]; do
  case $1 in
    -t|--build-type)
      BUILD_TYPE="$2"
      shift 2
      ;;
    -a|--action)
      ACTION="$2"
      shift 2
      ;;
    -clean|--clean)
      CLEAN_BUILD=true
      shift
      ;;
    -interactive|--interactive)
      INTERACTIVE_MODE=true
      shift
      ;;
    -h|--help)
      usage
      ;;
    *)
      echo "Unknown option: $1"
      usage
      ;;
  esac
done

# Check for interactive mode
if [ "$INTERACTIVE_MODE" = true ]; then
  interactive_mode
fi

# Check if Visual Studio environment is set up
if ! command -v cl &> /dev/null; then
  echo "Error: Visual Studio compiler 'cl' not found in PATH"
  echo "Please run this script from a Visual Studio Developer Command Prompt"
  echo "Or run: 'C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat'"
  echo "Then try again."
  exit 1
fi

# Check if CMake is available
if ! command -v cmake &> /dev/null; then
  echo "Error: CMake not found in PATH"
  echo "Please install CMake and ensure it's in your PATH"
  echo "Download from: https://cmake.org/download/"
  exit 1
fi

# Determine compiler version
if [ "$COMPILER" = "cl" ]; then
  COMPILER_VERSION=""
  if COMPILER_OUTPUT=$(cl 2>&1 | grep -i "version" | head -1); then
    COMPILER_VERSION=$(echo "$COMPILER_OUTPUT" | awk '{print $3}' | sed 's/[^0-9.]//g')
  fi
  if [ -z "$COMPILER_VERSION" ]; then
    if [ -d "/c/Program Files/Microsoft Visual Studio/2022" ]; then
      COMPILER_VERSION="2022"
    elif [ -d "/c/Program Files (x86)/Microsoft Visual Studio/2022" ]; then
      COMPILER_VERSION="2022"
    elif [ -d "/c/Program Files/Microsoft Visual Studio/2019" ]; then
      COMPILER_VERSION="2019"
    elif [ -d "/c/Program Files (x86)/Microsoft Visual Studio/2019" ]; then
      COMPILER_VERSION="2019"
    fi
  fi
  if [ -z "$COMPILER_VERSION" ]; then
    echo "Warning: Could not determine compiler version, using 'unknown'"
    COMPILER_VERSION="unknown"
  fi
else
  echo "Unsupported compiler: $COMPILER"
  exit 1
fi

echo "$PLATFORM :: $COMPILER-${COMPILER_VERSION}"

# Store project root directory
PROJECT_ROOT=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)

# Set the build directory based on build type and compiler
BUILD_DIR="$PROJECT_ROOT/build/${BUILD_TYPE}/build-windows-$COMPILER-${COMPILER_VERSION}"

# Build CMake command
build_cmake_command() {
  local args=()
  args+=("cmake")
  args+=("-G" "Visual Studio 17 2022")
  args+=("-A" "x64")
  args+=("-DCMAKE_BUILD_TYPE=$BUILD_TYPE")
  args+=("-DCMAKE_C_COMPILER=cl")
  args+=("-DCMAKE_CXX_COMPILER=cl")
  args+=("-DBUILD_TESTS=ON")
  args+=("$PROJECT_ROOT")
  "${args[@]}"
  return $?
}

BUILD_COMMAND="cmake --build . --config $BUILD_TYPE --parallel $JOBS"
TEST_COMMAND="ctest -C $BUILD_TYPE --output-on-failure"

# Clean previous build
clean_build() {
  rm -rf "$BUILD_DIR"
  mkdir -p "$BUILD_DIR"
  cd "$BUILD_DIR" || exit
}

# Ensure build directory exists (if not cleaning)
ensure_build_dir() {
  if [ ! -d "$BUILD_DIR" ]; then
    mkdir -p "$BUILD_DIR"
  fi
  cd "$BUILD_DIR" || exit
}

# Configure the project
configure_project() {
  echo "Configuring project..."
  build_cmake_command
  if [ $? -ne 0 ]; then
    echo "Error: CMake configuration failed"
    echo "Please check the error messages above and ensure:"
    echo "1. Visual Studio 2022 is installed"
    echo "2. CMake is installed and in PATH"
    echo "3. All required dependencies are available"
    exit 1
  fi
}

# Build the project
build_project() {
  echo "Building with $JOBS parallel jobs..."
  if ! eval $BUILD_COMMAND; then
    echo "Error: Build failed"
    echo "Please check the error messages above"
    exit 1
  fi
}

# Run tests
run_tests() {
  echo "Running tests..."
  if ! eval $TEST_COMMAND; then
    echo "Warning: Some tests failed"
    echo "Check the test output above for details"
  fi
}

# Perform actions based on the specified option
case $ACTION in
  configure)
    if [ "$CLEAN_BUILD" = true ]; then
      clean_build
    else
      ensure_build_dir
    fi
    configure_project
    ;;
  build)
    if [ "$CLEAN_BUILD" = true ]; then
      clean_build
    else
      ensure_build_dir
    fi
    configure_project
    build_project
    ;;
  configure_and_build)
    if [ "$CLEAN_BUILD" = true ]; then
      clean_build
    else
      ensure_build_dir
    fi
    configure_project
    build_project
    ;;
  test)
    if [ "$CLEAN_BUILD" = true ]; then
      clean_build
    else
      ensure_build_dir
    fi
    configure_project
    build_project
    run_tests
    ;;
  *)
    usage
    ;;
esac

echo ""
echo "=== Build completed successfully ==="
echo "Build directory: $BUILD_DIR"
