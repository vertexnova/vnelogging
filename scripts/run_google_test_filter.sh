#!/bin/bash

#==============================================================================
# Copyright (c) 2024 Ajeet Singh Yadav. All rights reserved.
# Licensed under the Apache License, Version 2.0 (the "License")
#
# Author:    Ajeet Singh Yadav
# Created:   January 2025
#
# Autodoc:   yes
#==============================================================================

# Simplified Google Test Filter Runner for VneLogging
# Usage: ./run_google_test_filter.sh <path/to/test_binary> [filter]

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

TEST_BINARY=""
FILTER=""
LIST_TESTS=false
VERBOSE=false

print_usage() {
    echo -e "${BLUE}Google Test Filter Runner for VneLogging${NC}"
    echo ""
    echo "Usage: $0 <path/to/test_binary> [filter] [options]"
    echo ""
    echo "Options:"
    echo "  -h, --help          Show this help message"
    echo "  -l, --list          List all available test cases"
    echo "  -v, --verbose       Enable verbose output"
    echo ""
    echo "Examples:"
    echo "  $0 build/Debug/build-linux-gcc-12.0.0/bin/TestVneLogging --list"
    echo "  $0 build/Debug/build-linux-gcc-12.0.0/bin/TestVneLogging \"LogStream.*\""
    echo "  $0 build/Debug/build-linux-gcc-12.0.0/bin/TestVneLogging \"*FileSink*\""
    echo "  $0 build/Debug/build-linux-gcc-12.0.0/bin/TestVneLogging \"Test1:Test2\""
    echo "  $0 build/Debug/build-linux-gcc-12.0.0/bin/TestVneLogging \"-*Slow*\""
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            print_usage
            exit 0
            ;;
        -l|--list)
            LIST_TESTS=true
            shift
            ;;
        -v|--verbose)
            VERBOSE=true
            shift
            ;;
        -*)
            echo -e "${RED}Error: Unknown option $1${NC}" >&2
            print_usage
            exit 1
            ;;
        *)
            if [[ -z "$TEST_BINARY" ]]; then
                TEST_BINARY="$1"
            elif [[ -z "$FILTER" ]]; then
                FILTER="$1"
            else
                echo -e "${RED}Error: Too many arguments${NC}" >&2
                print_usage
                exit 1
            fi
            shift
            ;;
    esac
done

if [[ -z "$TEST_BINARY" ]]; then
    echo -e "${RED}Error: Path to test binary is required${NC}" >&2
    print_usage
    exit 1
fi

if [[ ! -x "$TEST_BINARY" ]]; then
    echo -e "${RED}Error: Test binary '$TEST_BINARY' not found or not executable${NC}" >&2
    exit 1
fi

CMD="$TEST_BINARY"

if [[ -n "$FILTER" ]]; then
    CMD="$CMD --gtest_filter=\"$FILTER\""
fi

if [[ "$LIST_TESTS" == true ]]; then
    CMD="$CMD --gtest_list_tests"
fi

if [[ "$VERBOSE" == true ]]; then
    CMD="$CMD --gtest_verbose"
fi

CMD="$CMD --gtest_color=yes"

echo -e "${YELLOW}Executing: $CMD${NC}"
echo ""
eval "$CMD"
