#!/bin/bash

#==============================================================================
# VneLogging iOS Build Script
#==============================================================================
# Copyright (c) 2024 Ajeet Singh Yadav. All rights reserved.
# Licensed under the Apache License, Version 2.0 (the "License")
#
# Author:    Ajeet Singh Yadav
# Created:   January 2025
#
# This script builds VneLogging for iOS devices and simulator
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

# Platform detection (force iOS)
PLATFORM="iOS"
COMPILER="clang"

# Set PROJECT_ROOT robustly
PROJECT_ROOT=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)

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

# Function to display usage information
usage() {
  echo "Usage: $0 [-t <build_type>] [-a <action>] [-clean] [-interactive] [-j <jobs>] [-xcode] [-xcode-only] [-simulator] [-device] [-deployment-target <version>]"
  echo "Options:"
  echo "  -t <build_type>    Specify the build type (Debug, Release, RelWithDebInfo, MinSizeRel)"
  echo "  -a <action>        Specify the action to perform:"
  echo "                     configure: Configure the project"
  echo "                     build: Build the project"
  echo "                     configure_and_build: Configure and build the project"
  echo "                     test: Run tests"
  echo "                     xcode: Generate Xcode project only"
  echo "                     xcode_build: Generate Xcode project and build with xcodebuild"
  echo "  -clean             Clean the build directory before performing the action"
  echo "  -interactive       Run in interactive mode to choose options"
  echo "  -j <jobs>          Number of parallel jobs (default: 10)"
  echo "  -xcode             Generate Xcode project in addition to regular build"
  echo "  -xcode-only        Only generate Xcode project (skip regular build)"
  echo "  -simulator         Build for iOS Simulator (default)"
  echo "  -device            Build for iOS Device"
  echo "  -deployment-target Specify iOS deployment target (default: 15.0)"
  echo "                     Examples: 15.0, 16.0, 17.0, 18.0"
  echo ""
  echo "Examples:"
  echo "  $0 -t Debug -a configure_and_build"
  echo "  $0 -j 32"
  echo "  $0 -interactive"
  echo "  $0 -t Release -clean"
  echo "  $0 -a xcode -t Debug"
  echo "  $0 -xcode-only -t Release"
  echo "  $0 -device -t Debug"
  echo "  $0 -simulator -deployment-target 15.0"
  echo "  $0 -deployment-target 18.0 -device"
  exit 1
}

# Function to check iOS development environment
check_ios_environment() {
  echo "=== Checking iOS Development Environment ==="

  # Check if Xcode is installed
  if ! command -v xcodebuild &> /dev/null; then
    echo "ERROR: Xcode not found. Please install Xcode from the App Store."
    exit 1
  fi

  # Check Xcode version
  XCODE_VERSION=$(xcodebuild -version | head -n 1)
  echo "Found: $XCODE_VERSION"

  # Check if iOS SDK is available
  if ! xcodebuild -showsdks | grep -q "iphoneos"; then
    echo "ERROR: iOS SDK not found. Please install iOS SDK in Xcode."
    exit 1
  fi

  # Check if iOS Simulator SDK is available
  if ! xcodebuild -showsdks | grep -q "iphonesimulator"; then
    echo "ERROR: iOS Simulator SDK not found. Please install iOS Simulator SDK in Xcode."
    exit 1
  fi

  echo "iOS development environment check passed."
  echo ""
}

# Function to get iOS SDK paths
get_ios_sdk_paths() {
  if [ "$TARGET" = "device" ]; then
    IOS_SDK=$(xcodebuild -showsdks | grep iphoneos | head -1 | sed 's/.*iphoneos\([0-9.]*\)/\1/' | tr -d ' ')
    IOS_SDK_PATH=$(xcrun --sdk iphoneos --show-sdk-path)
    echo "Using iOS Device SDK: $IOS_SDK_PATH (version $IOS_SDK)"
  else
    IOS_SDK=$(xcodebuild -showsdks | grep iphonesimulator | head -1 | sed 's/.*iphonesimulator\([0-9.]*\)/\1/' | tr -d ' ')
    IOS_SDK_PATH=$(xcrun --sdk iphonesimulator --show-sdk-path)
    echo "Using iOS Simulator SDK: $IOS_SDK_PATH (version $IOS_SDK)"
  fi

  # Validate deployment target format
  if [[ ! $IOS_DEPLOYMENT_TARGET =~ ^[0-9]+\.[0-9]+$ ]]; then
    echo "ERROR: Invalid iOS deployment target format: $IOS_DEPLOYMENT_TARGET"
    echo "Expected format: X.Y (e.g., 15.0, 16.0, 17.0, 18.0)"
    exit 1
  fi

  echo "Using iOS deployment target: $IOS_DEPLOYMENT_TARGET"
}

# Initialize variables
BUILD_TYPE="Debug"
ACTION="configure_and_build"
CLEAN=false
TARGET="simulator"
XCODE_ONLY=false
GENERATE_XCODE=false
IOS_DEPLOYMENT_TARGET="15.0"  # Default deployment target

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -t|--type)
            BUILD_TYPE="$2"
            shift 2
            ;;
        -a|--action)
            ACTION="$2"
            shift 2
            ;;
        -clean)
            CLEAN=true
            shift
            ;;
        -interactive)
            echo "Interactive mode not yet implemented for iOS"
            usage
            ;;
        -xcode)
            GENERATE_XCODE=true
            shift
            ;;
        -xcode-only)
            XCODE_ONLY=true
            ACTION="xcode"
            shift
            ;;
        -simulator)
            TARGET="simulator"
            shift
            ;;
        -device)
            TARGET="device"
            shift
            ;;
        -deployment-target)
            IOS_DEPLOYMENT_TARGET="$2"
            shift 2
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

# Check iOS development environment
check_ios_environment

# Get iOS SDK paths
get_ios_sdk_paths

# Set up build directories as absolute paths
BUILD_DIR="$PROJECT_ROOT/build/${BUILD_TYPE}/build-ios-$COMPILER-${COMPILER_VERSION}"
XCODE_DIR="$PROJECT_ROOT/build/${BUILD_TYPE}/xcode-ios-$COMPILER-${COMPILER_VERSION}"

# Clean build directory if requested
if [ "$CLEAN" = true ]; then
    echo "Cleaning build directory: $BUILD_DIR"
    rm -rf "$BUILD_DIR"
    echo "Cleaning Xcode directory: $XCODE_DIR"
    rm -rf "$XCODE_DIR"
fi

# Create build directories
mkdir -p "$BUILD_DIR"
mkdir -p "$XCODE_DIR"

# Set up CMake configuration
CMAKE_ARGS=(
    "-DCMAKE_BUILD_TYPE=$BUILD_TYPE"
    "-DVNE_TARGET_PLATFORM=iOS"
    "-DCMAKE_SYSTEM_NAME=iOS"
    "-DCMAKE_OSX_DEPLOYMENT_TARGET=$IOS_DEPLOYMENT_TARGET"
    "-DCMAKE_OSX_ARCHITECTURES=arm64"
    "-DCMAKE_C_COMPILER=clang"
    "-DCMAKE_CXX_COMPILER=clang++"
    "-DCMAKE_CXX_STANDARD=20"
    "-DCMAKE_CXX_STANDARD_REQUIRED=ON"
    "-DCMAKE_CXX_EXTENSIONS=OFF"
    "-DBUILD_TESTS=OFF"  # Tests disabled for iOS
)

# Set target-specific flags
if [ "$TARGET" = "device" ]; then
    CMAKE_ARGS+=(
        "-DCMAKE_OSX_SYSROOT=iphoneos"
        "-DCMAKE_OSX_ARCHITECTURES=arm64"
    )
else
    CMAKE_ARGS+=(
        "-DCMAKE_OSX_SYSROOT=iphonesimulator"
        "-DCMAKE_OSX_ARCHITECTURES=arm64;x86_64"
    )
fi

# Function to run CMake configuration (always Xcode for iOS)
run_cmake_configure() {
    local build_path="$1"
    echo "Configuring CMake (Xcode) in: $build_path"
    echo "CMake arguments: ${CMAKE_ARGS[*]}"

    cd "$build_path"
    cmake -G "Xcode" "${CMAKE_ARGS[@]}" "$PROJECT_ROOT"

    if [ $? -ne 0 ]; then
        echo "ERROR: CMake configuration failed"
        exit 1
    fi
    cd - > /dev/null
}

# Function to build with Xcode
run_xcode_build() {
    local xcode_path="$1"
    echo "Building with Xcode in: $xcode_path"

    cd "$xcode_path"

    # Set up Xcode build configuration
    local xcode_config="Debug"
    if [ "$BUILD_TYPE" = "Release" ]; then
        xcode_config="Release"
    fi

    # Set up target
    local xcode_target="iphoneos"
    if [ "$TARGET" = "simulator" ]; then
        xcode_target="iphonesimulator"
    fi

    # Find the .xcodeproj directory
    local xcodeproj_file=$(find . -maxdepth 1 -name "*.xcodeproj" -type d | head -n1)
    if [ -z "$xcodeproj_file" ]; then
        echo "ERROR: No Xcode project found in $xcode_path"
        exit 1
    fi
    xcodeproj_file=$(basename "$xcodeproj_file")

    xcodebuild -project "$xcodeproj_file" -scheme ALL_BUILD -configuration "$xcode_config" -sdk "$xcode_target" -jobs "$JOBS"

    if [ $? -ne 0 ]; then
        echo "ERROR: Xcode build failed"
        exit 1
    fi
    cd - > /dev/null
}

# Main execution
echo "=== VneLogging iOS Build Script ==="
echo "Platform: $PLATFORM"
echo "Compiler: $COMPILER"
echo "Build Type: $BUILD_TYPE"
echo "Target: $TARGET"
echo "iOS Deployment Target: $IOS_DEPLOYMENT_TARGET"
echo "Action: $ACTION"
echo "Generate Xcode: $GENERATE_XCODE"
echo "Jobs: $JOBS"
echo ""

# Execute based on action
case $ACTION in
    "configure")
        run_cmake_configure "$BUILD_DIR"
        ;;
    "build"|"configure_and_build")
        run_cmake_configure "$BUILD_DIR"
        run_xcode_build "$BUILD_DIR"
        # Also generate Xcode project if requested
        if [ "$GENERATE_XCODE" = true ]; then
            run_cmake_configure "$XCODE_DIR"
        fi
        ;;
    "xcode_build")
        run_cmake_configure "$BUILD_DIR"
        run_xcode_build "$BUILD_DIR"
        ;;
    "xcode"|"xcode-only")
        run_cmake_configure "$XCODE_DIR"
        ;;
    "test")
        echo "Note: Tests are disabled for iOS builds"
        run_cmake_configure "$BUILD_DIR"
        run_xcode_build "$BUILD_DIR"
        ;;
    *)
        echo "ERROR: Unknown action: $ACTION"
        usage
        ;;
esac

echo ""
echo "=== Build completed successfully ==="
if [ "$ACTION" = "xcode" ] || [ "$ACTION" = "xcode-only" ]; then
    echo "Xcode project generated in: $XCODE_DIR"
    echo "To open in Xcode: open $XCODE_DIR/*.xcodeproj"
else
    echo "Xcode project generated in: $BUILD_DIR"
    echo "To open in Xcode: open $BUILD_DIR/*.xcodeproj"
    if [ "$GENERATE_XCODE" = true ]; then
        echo "Additional Xcode project in: $XCODE_DIR"
    fi
fi
echo "Build artifacts in: $BUILD_DIR"
echo "Target: $TARGET (iOS $IOS_DEPLOYMENT_TARGET)"
