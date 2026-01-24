#!/bin/bash

#==============================================================================
# Coverage Report Generation Script for VneLogging
# Supports macOS (LLVM) and Linux (GCC/lcov)
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
BUILD_TYPE="Debug"
FILTER_SRC_ONLY=false
OPEN_REPORT=true
EXECUTABLE=""
RUN_TESTS=true

# Function to display usage information
usage() {
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Generate code coverage reports for VneLogging"
    echo ""
    echo "Options:"
    echo "  -d, --build-dir <dir>     Build directory (required)"
    echo "  -t, --build-type <type>   Build type (Debug, Release) [default: Debug]"
    echo "  -e, --executable <exe>    Test executable to use [default: auto-detect]"
    echo "  -f, --filter-src-only     Filter to only src/vnelogging (exclude tests, 3rd_party)"
    echo "  -n, --no-open             Don't open report in browser"
    echo "  --no-tests                Don't run tests (use existing coverage data)"
    echo "  -h, --help                Show this help message"
    echo ""
    echo "Note: By default, the script runs 'ctest' to generate fresh coverage data."
    echo "      Use --no-tests to skip test execution and use existing coverage data."
    echo ""
    echo "Examples:"
    echo "  $0 -d build/Debug/build-macos-clang-17.0.0"
    echo "  $0 -d build/Debug/build-linux-gcc-12.0.0 -f"
    echo "  $0 -d build/Debug/build-macos-clang-17.0.0 -e ./bin/TestVneLogging -f"
    echo "  $0 -d build/Debug/build-macos-clang-17.0.0 --no-tests"
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

# Function to check prerequisites
check_prerequisites() {
    local missing_tools=()

    if [[ "$PLATFORM" == "macos" ]]; then
        if ! command -v xcrun &> /dev/null; then
            missing_tools+=("xcrun (Xcode Command Line Tools)")
        fi
        if ! xcrun -f llvm-profdata &> /dev/null; then
            missing_tools+=("llvm-profdata")
        fi
        if ! xcrun -f llvm-cov &> /dev/null; then
            missing_tools+=("llvm-cov")
        fi
    elif [[ "$PLATFORM" == "linux" ]]; then
        if ! command -v lcov &> /dev/null; then
            missing_tools+=("lcov")
        fi
        if ! command -v genhtml &> /dev/null; then
            missing_tools+=("genhtml")
        fi
    fi

    if [[ ${#missing_tools[@]} -gt 0 ]]; then
        echo -e "${RED}❌ Missing required tools:${NC}"
        for tool in "${missing_tools[@]}"; do
            echo -e "   ${RED}- $tool${NC}"
        done
        echo ""
        if [[ "$PLATFORM" == "macos" ]]; then
            echo "Install Xcode Command Line Tools: xcode-select --install"
        elif [[ "$PLATFORM" == "linux" ]]; then
            echo "Install lcov: sudo apt-get install lcov (Ubuntu/Debian)"
            echo "             sudo yum install lcov (RHEL/CentOS)"
        fi
        exit 1
    fi
}

# Function to check if coverage is enabled
check_coverage_enabled() {
    local cmake_cache="${BUILD_DIR}/CMakeCache.txt"

    if [[ ! -f "$cmake_cache" ]]; then
        echo -e "${RED}❌ CMakeCache.txt not found in: $BUILD_DIR${NC}"
        exit 1
    fi

    if grep -q "ENABLE_COVERAGE:BOOL=ON" "$cmake_cache"; then
        echo -e "${GREEN}✓ Coverage is enabled${NC}"
        return 0
    else
        echo -e "${RED}❌ Coverage is not enabled!${NC}"
        echo -e "${YELLOW}   Reconfigure CMake with: -DENABLE_COVERAGE=ON${NC}"
        exit 1
    fi
}

# Function to find test executable
find_test_executable() {
    local bin_dir="${BUILD_DIR}/bin"

    if [[ -n "$EXECUTABLE" ]]; then
        if [[ -f "$EXECUTABLE" ]]; then
            echo "$EXECUTABLE"
            return 0
        elif [[ -f "${BUILD_DIR}/${EXECUTABLE}" ]]; then
            echo "${BUILD_DIR}/${EXECUTABLE}"
            return 0
        else
            echo -e "${RED}❌ Executable not found: $EXECUTABLE${NC}"
            exit 1
        fi
    fi

    # Auto-detect test executables
    local test_execs=(
        "${bin_dir}/TestVneLogging"
    )

    for exe in "${test_execs[@]}"; do
        if [[ -f "$exe" && -x "$exe" ]]; then
            echo "$exe"
            return 0
        fi
    done

    # Try to find any test executable
    while IFS= read -r exe; do
        if [[ -f "$exe" && -x "$exe" ]]; then
            echo "$exe"
            return 0
        fi
    done < <(find "$bin_dir" -type f -perm +111 \( -name "*test*" -o -name "*Test*" \) 2>/dev/null || true)

    echo -e "${RED}❌ No test executable found in: $bin_dir${NC}"
    echo -e "${YELLOW}   Specify with: -e <executable>${NC}"
    exit 1
}

# Function to run tests if needed
run_tests_if_needed() {
    if [[ "$RUN_TESTS" == false ]]; then
        echo -e "${BLUE}   Skipping test execution (--no-tests specified)${NC}"
        return 0
    fi

    echo -e "${BLUE}   Running tests to generate coverage data...${NC}"
    echo -e "${YELLOW}   This may take a while...${NC}"

    local original_dir=$(pwd)
    cd "$BUILD_DIR"

    if ctest --output-on-failure; then
        echo -e "${GREEN}   ✓ Tests completed successfully${NC}"
        cd "$original_dir"
        return 0
    else
        echo -e "${YELLOW}   ⚠ Some tests failed, but continuing with coverage report${NC}"
        echo -e "${YELLOW}   Coverage data may be incomplete${NC}"
        cd "$original_dir"
        return 0
    fi
}

# Function to generate macOS coverage report
generate_macos_coverage() {
    local test_exe="$1"
    local report_dir="${BUILD_DIR}/coverage_report"

    if [[ "$FILTER_SRC_ONLY" == true ]]; then
        report_dir="${BUILD_DIR}/coverage_report_filtered"
    fi

    echo -e "${BLUE}📊 Generating macOS (LLVM) coverage report...${NC}"

    # Run tests if needed
    run_tests_if_needed

    # Find all profraw files
    echo -e "${BLUE}   Finding coverage data files...${NC}"
    local profraw_files=$(find "$BUILD_DIR" -name "*.profraw" 2>/dev/null)
    local profraw_count=$(echo "$profraw_files" | grep -c "\.profraw" || echo "0")

    if [[ "$profraw_count" -eq 0 ]]; then
        echo -e "${RED}❌ No .profraw files found!${NC}"
        echo -e "${YELLOW}   Run tests first: ctest${NC}"
        exit 1
    fi

    echo -e "${GREEN}   ✓ Found $profraw_count .profraw file(s)${NC}"

    # Merge profraw files
    echo -e "${BLUE}   Merging coverage data...${NC}"
    local profdata="${BUILD_DIR}/coverage.profdata"
    xcrun llvm-profdata merge -sparse $profraw_files -o "$profdata" 2>&1 | grep -v "^$" || true

    if [[ ! -f "$profdata" ]]; then
        echo -e "${RED}❌ Failed to merge profraw files${NC}"
        exit 1
    fi

    echo -e "${GREEN}   ✓ Merged coverage data${NC}"

    # Generate HTML report
    echo -e "${BLUE}   Generating HTML report...${NC}"
    mkdir -p "$report_dir"

    local cov_args=(
        "$test_exe"
        -instr-profile="$profdata"
        -format=html
        -output-dir="$report_dir"
        -show-line-counts-or-regions
    )

    if [[ "$FILTER_SRC_ONLY" == true ]]; then
        cov_args+=(-ignore-filename-regex='tests|googletest|gtest|gmock|3rd_party')
    fi

    xcrun llvm-cov show "${cov_args[@]}" 2>&1 | grep -E "(error|warning)" || true

    if [[ ! -f "${report_dir}/index.html" ]]; then
        echo -e "${RED}❌ Failed to generate HTML report${NC}"
        exit 1
    fi

    echo -e "${GREEN}   ✓ HTML report generated${NC}"

    # Generate summary
    echo ""
    echo -e "${BLUE}📈 Coverage Summary:${NC}"

    local summary_args=(
        "$test_exe"
        -instr-profile="$profdata"
    )

    if [[ "$FILTER_SRC_ONLY" == true ]]; then
        summary_args+=(-ignore-filename-regex='tests|googletest|gtest|gmock|3rd_party')
        xcrun llvm-cov report "${summary_args[@]}" 2>&1 | grep -E "^src/vnelogging|^TOTAL" | head -20
    else
        xcrun llvm-cov report "${summary_args[@]}" 2>&1 | tail -5
    fi

    echo ""
    echo -e "${GREEN}📊 Report location: ${report_dir}/index.html${NC}"

    if [[ "$OPEN_REPORT" == true ]]; then
        echo -e "${BLUE}   Opening report in browser...${NC}"
        open "$report_dir/index.html" 2>/dev/null || xdg-open "$report_dir/index.html" 2>/dev/null || true
    fi
}

# Function to generate Linux coverage report
generate_linux_coverage() {
    local test_exe="$1"
    local report_dir="${BUILD_DIR}/coverage_report"

    if [[ "$FILTER_SRC_ONLY" == true ]]; then
        report_dir="${BUILD_DIR}/coverage_report_filtered"
    fi

    echo -e "${BLUE}📊 Generating Linux (GCC/lcov) coverage report...${NC}"

    # Check for .gcda files
    echo -e "${BLUE}   Finding coverage data files...${NC}"
    local gcda_count=$(find "$BUILD_DIR" -name "*.gcda" 2>/dev/null | wc -l)

    if [[ "$gcda_count" -eq 0 ]]; then
        echo -e "${RED}❌ No .gcda files found!${NC}"
        echo -e "${YELLOW}   Run tests first: ctest${NC}"
        exit 1
    fi

    echo -e "${GREEN}   ✓ Found $gcda_count .gcda file(s)${NC}"

    # Capture coverage
    echo -e "${BLUE}   Capturing coverage data...${NC}"
    local coverage_info="${BUILD_DIR}/coverage.info"
    lcov --capture --directory "$BUILD_DIR" --output-file "$coverage_info" 2>&1 | grep -v "^$" || true

    if [[ ! -f "$coverage_info" ]]; then
        echo -e "${RED}❌ Failed to capture coverage data${NC}"
        exit 1
    fi

    # Filter coverage
    echo -e "${BLUE}   Filtering coverage data...${NC}"
    local filtered_info="${BUILD_DIR}/coverage_filtered.info"
    local filter_args=(
        --remove "$coverage_info"
        '/usr/*'
        '*/tests/*'
        '*/3rd_party/*'
        '*/googletest/*'
        '*/gtest/*'
        '*/gmock/*'
    )

    if [[ "$FILTER_SRC_ONLY" == true ]]; then
        filter_args+=(--extract "$coverage_info" 'src/vertexnova/logging/*')
    fi

    filter_args+=(--output-file "$filtered_info")

    lcov "${filter_args[@]}" 2>&1 | grep -v "^$" || true

    # Generate HTML report
    echo -e "${BLUE}   Generating HTML report...${NC}"
    mkdir -p "$report_dir"
    genhtml "$filtered_info" --output-directory "$report_dir" 2>&1 | grep -E "(error|warning|lines)" || true

    if [[ ! -f "${report_dir}/index.html" ]]; then
        echo -e "${RED}❌ Failed to generate HTML report${NC}"
        exit 1
    fi

    echo -e "${GREEN}   ✓ HTML report generated${NC}"

    # Generate summary
    echo ""
    echo -e "${BLUE}📈 Coverage Summary:${NC}"
    lcov --summary "$filtered_info" 2>&1 | tail -5

    echo ""
    echo -e "${GREEN}📊 Report location: ${report_dir}/index.html${NC}"

    if [[ "$OPEN_REPORT" == true ]]; then
        echo -e "${BLUE}   Opening report in browser...${NC}"
        xdg-open "$report_dir/index.html" 2>/dev/null || true
    fi
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -d|--build-dir)
            BUILD_DIR="$2"
            shift 2
            ;;
        -t|--build-type)
            BUILD_TYPE="$2"
            shift 2
            ;;
        -e|--executable)
            EXECUTABLE="$2"
            shift 2
            ;;
        -f|--filter-src-only)
            FILTER_SRC_ONLY=true
            shift
            ;;
        -n|--no-open)
            OPEN_REPORT=false
            shift
            ;;
        --no-tests)
            RUN_TESTS=false
            shift
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
echo -e "${BLUE}=== VneLogging Coverage Report Generator ===${NC}"
echo ""

# Detect platform
detect_platform
echo -e "${GREEN}✓ Platform: $PLATFORM${NC}"

# Check prerequisites
check_prerequisites

# Check if coverage is enabled
check_coverage_enabled

# Find test executable
TEST_EXECUTABLE=$(find_test_executable)
echo -e "${GREEN}✓ Using executable: $TEST_EXECUTABLE${NC}"
echo ""

# Generate coverage report
if [[ "$PLATFORM" == "macos" ]]; then
    generate_macos_coverage "$TEST_EXECUTABLE"
elif [[ "$PLATFORM" == "linux" ]]; then
    generate_linux_coverage "$TEST_EXECUTABLE"
fi

echo ""
echo -e "${GREEN}✅ Coverage report generation complete!${NC}"
