#!/bin/bash

#==============================================================================
# Coverage Verification Script for VneLogging
# Checks if coverage is enabled and working
#==============================================================================
# Copyright (c) 2024 Ajeet Singh Yadav. All rights reserved.
# Licensed under the Apache License, Version 2.0 (the "License")
#
# Author:    Ajeet Singh Yadav
# Created:   January 2025
#==============================================================================

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Default values
BUILD_DIR=""

# Function to display usage information
usage() {
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Check if code coverage is enabled and working"
    echo ""
    echo "Options:"
    echo "  -d, --build-dir <dir>     Build directory (required)"
    echo "  -h, --help                Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0 -d build/Debug/build-macos-clang-17.0.0"
    echo "  $0 -d build/Debug/build-linux-gcc-12.0.0"
}

# Function to detect platform
detect_platform() {
    if [[ "$OSTYPE" == "darwin"* ]]; then
        PLATFORM="macos"
    elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
        PLATFORM="linux"
    else
        echo -e "${RED}❌ Unsupported platform: $OSTYPE${NC}"
        exit 1
    fi
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -d|--build-dir)
            BUILD_DIR="$2"
            shift 2
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        *)
            echo -e "${RED}❌ Unknown option: $1${NC}"
            usage
            exit 1
            ;;
    esac
done

# Validate required arguments
if [[ -z "$BUILD_DIR" ]]; then
    echo -e "${RED}❌ Build directory is required${NC}"
    usage
    exit 1
fi

# Convert to absolute path
BUILD_DIR=$(cd "$BUILD_DIR" 2>/dev/null && pwd || echo "$BUILD_DIR")

if [[ ! -d "$BUILD_DIR" ]]; then
    echo -e "${RED}❌ Build directory does not exist: $BUILD_DIR${NC}"
    exit 1
fi

# Main execution
echo -e "${BLUE}=== Coverage Verification ===${NC}"
echo ""

# Detect platform
detect_platform
echo -e "${GREEN}✓ Platform: $PLATFORM${NC}"

# Check CMake cache
local cmake_cache="${BUILD_DIR}/CMakeCache.txt"

if [[ ! -f "$cmake_cache" ]]; then
    echo -e "${RED}❌ CMakeCache.txt not found in: $BUILD_DIR${NC}"
    exit 1
fi

# Check if coverage is enabled
if grep -q "ENABLE_COVERAGE:BOOL=ON" "$cmake_cache"; then
    echo -e "${GREEN}✓ Coverage is enabled${NC}"
else
    echo -e "${RED}❌ Coverage is not enabled!${NC}"
    echo -e "${YELLOW}   Reconfigure CMake with: -DENABLE_COVERAGE=ON${NC}"
    exit 1
fi

# Check coverage flags in compile commands
if [[ "$PLATFORM" == "macos" ]]; then
    # macOS: Check for coverage flags in compile commands
    if grep -q "fprofile-instr-generate\|fcoverage-mapping" "$cmake_cache" 2>/dev/null; then
        echo -e "${GREEN}✓ Coverage flags detected in CMake cache${NC}"
    else
        echo -e "${YELLOW}⚠ Coverage flags not found in CMake cache${NC}"
    fi

    # Check for .profraw files
    profraw_count=$(find "$BUILD_DIR" -name "*.profraw" 2>/dev/null | wc -l)
    echo -e "${BLUE}   Found $profraw_count .profraw file(s)${NC}"
elif [[ "$PLATFORM" == "linux" ]]; then
    # Linux: Check for coverage flags
    if grep -q "coverage\|--coverage" "$cmake_cache" 2>/dev/null; then
        echo -e "${GREEN}✓ Coverage flags detected in CMake cache${NC}"
    else
        echo -e "${YELLOW}⚠ Coverage flags not found in CMake cache${NC}"
    fi

    # Check for .gcda and .gcno files
    gcda_count=$(find "$BUILD_DIR" -name "*.gcda" 2>/dev/null | wc -l)
    gcno_count=$(find "$BUILD_DIR" -name "*.gcno" 2>/dev/null | wc -l)
    echo -e "${BLUE}   Found $gcda_count .gcda file(s) and $gcno_count .gcno file(s)${NC}"
fi

# List test executables
echo ""
echo -e "${BLUE}Test Executables:${NC}"
bin_dir="${BUILD_DIR}/bin"
if [[ -d "$bin_dir" ]]; then
    test_execs=$(find "$bin_dir" -type f -perm +111 \( -name "*test*" -o -name "*Test*" \) 2>/dev/null || true)
    if [[ -n "$test_execs" ]]; then
        while IFS= read -r exe; do
            if [[ -f "$exe" && -x "$exe" ]]; then
                echo -e "${GREEN}   ✓ $(basename $exe)${NC}"
            fi
        done <<< "$test_execs"
    else
        echo -e "${YELLOW}   ⚠ No test executables found${NC}"
    fi
else
    echo -e "${YELLOW}   ⚠ bin directory not found${NC}"
fi

echo ""
echo -e "${GREEN}✅ Coverage verification complete!${NC}"
echo ""
echo -e "${BLUE}Next steps:${NC}"
echo "  1. Run tests: cd $BUILD_DIR && ctest"
echo "  2. Generate coverage report: ./scripts/generate_coverage.sh -d $BUILD_DIR"
