#!/bin/bash

#==============================================================================
# VneLogging macOS Build Script
#==============================================================================
# Copyright (c) 2024 Ajeet Singh Yadav. All rights reserved.
# Licensed under the Apache License, Version 2.0 (the "License")
#
# Author:    Ajeet Singh Yadav
# Created:   January 2025
#
# This script builds VneLogging for macOS with Xcode integration
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

# Platform detection (force macOS)
PLATFORM="Darwin"
COMPILER="clang"

# Function to display usage information
usage() {
  echo "Usage: $0 [-t <build_type>] [-a <action>] [-clean] [-interactive] [-j <jobs>] [-xcode] [-xcode-only]"
  echo "Options:"
  echo "  -t <build_type>    Specify the build type (Debug, Release, RelWithDebInfo, MinSizeRel)"
  echo "  -a <action>        Specify the action to perform:"
  echo "                     configure: Configure the project"
  echo "                     build: Build the project"
  echo "                     configure_and_build: Configure and build the project"
  echo "                     test: Run tests"
  echo "                     xcode: Generate Xcode project"
  echo "                     xcode_build: Generate Xcode project and build with xcodebuild"
  echo "  -clean             Clean the build directory before performing the action"
  echo "  -interactive       Run in interactive mode to choose options"
  echo "  -j <jobs>          Number of parallel jobs (default: 10)"
  echo "  -xcode             Generate Xcode project in addition to regular build"
  echo "  -xcode-only        Only generate Xcode project (skip regular build)"
  echo ""
  echo "Examples:"
  echo "  $0 -t Debug -a configure_and_build"
  echo "  $0 -j 32"
  echo "  $0 -interactive"
  echo "  $0 -t Release -clean"
  echo "  $0 -a xcode -t Debug"
  echo "  $0 -xcode-only -t Release"
  exit 1
}

# Function to display Xcode information
show_xcode_info() {
  echo ""
  echo "=== Xcode Project Generation ==="
  echo "Xcode projects are generated in separate folders:"
  echo "  - Regular builds: build/<type>/build-macos-<compiler>-<version>/"
  echo "  - Xcode projects: build/<type>/xcode-macos-<compiler>-<version>/"
  echo ""
  echo "Benefits of Xcode projects:"
  echo "  - Full IDE integration with debugging"
  echo "  - IntelliSense and code completion"
  echo "  - Integrated build system"
  echo "  - Easy project navigation"
  echo ""
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
  echo "4) Generate Xcode project"
  echo "5) Generate Xcode project and build with xcodebuild"
  read -p "Enter choice (1-5) [2]: " action_choice
  case $action_choice in
    1) ACTION="configure" ;;
    3) ACTION="test" ;;
    4) ACTION="xcode" ;;
    5) ACTION="xcode_build" ;;
    *) ACTION="configure_and_build" ;;
  esac

  # Clean build option
  echo ""
  read -p "Clean build directory before starting? (y/N): " clean_choice
  if [[ $clean_choice =~ ^[Yy]$ ]]; then
    CLEAN_BUILD=true
  fi

  # Xcode generation option
  echo ""
  show_xcode_info
  read -p "Generate Xcode project? (y/N): " xcode_choice
  if [[ $xcode_choice =~ ^[Yy]$ ]]; then
    GENERATE_XCODE=true
    if [ "$ACTION" = "configure_and_build" ]; then
      ACTION="xcode_build"
    fi
  fi

  echo ""
  echo "=== Configuration Summary ==="
  echo "Platform: $PLATFORM"
  echo "Build Type: $BUILD_TYPE"
  echo "Compiler: $COMPILER"
  echo "Action: $ACTION"
  echo "Clean Build: $CLEAN_BUILD"
  echo "Parallel Jobs: $JOBS"
  if [ "$GENERATE_XCODE" = true ]; then
    echo "Xcode Project: Enabled"
  fi
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
COMPILER="clang"
CLEAN_BUILD=false
INTERACTIVE_MODE=false
GENERATE_XCODE=false
XCODE_ONLY=false

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
    -xcode|--xcode)
      GENERATE_XCODE=true
      shift
      ;;
    -xcode-only|--xcode-only)
      XCODE_ONLY=true
      GENERATE_XCODE=true
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

# Handle Xcode flags
if [ "$XCODE_ONLY" = true ]; then
  ACTION="xcode"
elif [ "$GENERATE_XCODE" = true ] && [ "$ACTION" = "configure_and_build" ]; then
  ACTION="xcode_build"
fi

# Set GENERATE_XCODE flag based on action
if [ "$ACTION" = "xcode" ] || [ "$ACTION" = "xcode_build" ]; then
  GENERATE_XCODE=true
fi

# Determine compiler version
if [ "$COMPILER" = "clang" ]; then
  COMPILER_VERSION=$(clang --version | head -n 1 | awk '{print $4}' | sed 's/(.*)//')
  if [ "$COMPILER_VERSION" = "version" ]; then
    COMPILER_VERSION=$(clang --version | head -n 1 | awk '{print $3}')
  fi
else
  echo "Unsupported compiler: $COMPILER"
  exit 1
fi

echo "$PLATFORM :: $COMPILER-${COMPILER_VERSION}"

# Store project root directory
PROJECT_ROOT=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)

# Set the build directory based on build type and compiler
if [ "$GENERATE_XCODE" = true ]; then
  BUILD_DIR="$PROJECT_ROOT/build/${BUILD_TYPE}/xcode-macos-$COMPILER-${COMPILER_VERSION}"
else
  BUILD_DIR="$PROJECT_ROOT/build/${BUILD_TYPE}/build-macos-$COMPILER-${COMPILER_VERSION}"
fi

# Build CMake command
build_cmake_command() {
  local base_cmd=""

  # Choose generator based on Xcode flag
  if [ "$GENERATE_XCODE" = true ]; then
    base_cmd="cmake -G Xcode -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 -DBUILD_TESTS=ON"
  else
    base_cmd="cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 -DBUILD_TESTS=ON"
  fi

  echo "$base_cmd $PROJECT_ROOT"
}

CONFIGURE_COMMAND=$(build_cmake_command)

# Set build command based on generator
if [ "$GENERATE_XCODE" = true ]; then
  BUILD_COMMAND="xcodebuild -project vnelogging.xcodeproj -configuration $BUILD_TYPE -parallelizeTargets -jobs $JOBS"
  TEST_COMMAND="xcodebuild -project vnelogging.xcodeproj -configuration $BUILD_TYPE -target RUN_TESTS"
else
  BUILD_COMMAND="make -j$JOBS"
  TEST_COMMAND="ctest --output-on-failure"
fi

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
  echo "Configuring with command: $CONFIGURE_COMMAND"
  eval $CONFIGURE_COMMAND
}

# Build the project
build_project() {
  echo "Building with $JOBS parallel jobs..."
  eval $BUILD_COMMAND
}

# Run tests
run_tests() {
  eval $TEST_COMMAND
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
  xcode)
    if [ "$CLEAN_BUILD" = true ]; then
      clean_build
    else
      ensure_build_dir
    fi
    configure_project
    echo "Xcode project generated successfully in: $BUILD_DIR"
    echo "You can now open vnelogging.xcodeproj in Xcode"
    ;;
  xcode_build)
    if [ "$CLEAN_BUILD" = true ]; then
      clean_build
    else
      ensure_build_dir
    fi
    configure_project
    build_project
    echo "Xcode project built successfully in: $BUILD_DIR"
    ;;
  *)
    usage
    ;;
esac

echo ""
echo "=== Build completed successfully ==="
echo "Build directory: $BUILD_DIR"
