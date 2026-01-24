#!/bin/bash

#==============================================================================
# VneLogging Documentation Generation Script
# This script automates the documentation generation and validation process
#==============================================================================
# Copyright (c) 2024 Ajeet Singh Yadav. All rights reserved.
# Licensed under the Apache License, Version 2.0 (the "License")
#
# Author:    Ajeet Singh Yadav
# Created:   January 2025
#==============================================================================

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
DOCS_DIR="$PROJECT_ROOT/docs"
BUILD_DIR="$PROJECT_ROOT/build"
DOXYGEN_OUTPUT="$DOCS_DIR/api/generated"

# Logging functions
log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check prerequisites
check_prerequisites() {
    log_info "Checking prerequisites..."

    # Check if we're in the right directory
    if [[ ! -f "$PROJECT_ROOT/CMakeLists.txt" ]]; then
        log_error "Not in VneLogging project root. Please run from project root."
        exit 1
    fi

    # Check for required tools
    if ! command -v cmake &> /dev/null; then
        log_error "CMake not found. Please install CMake."
        exit 1
    fi

    if ! command -v doxygen &> /dev/null; then
        log_warning "Doxygen not found. API documentation will not be generated."
        DOXYGEN_AVAILABLE=false
    else
        DOXYGEN_AVAILABLE=true
    fi

    if ! command -v markdown-link-check &> /dev/null; then
        log_warning "markdown-link-check not found. Link validation will be skipped."
        LINK_CHECK_AVAILABLE=false
    else
        LINK_CHECK_AVAILABLE=true
    fi

    log_success "Prerequisites check completed"
}

# Generate API documentation
generate_api_docs() {
    if [[ "$DOXYGEN_AVAILABLE" == "false" ]]; then
        log_warning "Skipping API documentation generation (Doxygen not available)"
        return
    fi

    log_info "Generating API documentation..."

    # Create build directory if it doesn't exist
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"

    # Configure CMake with documentation enabled (if Doxygen target exists)
    # For now, we'll use Doxygen directly
    if [[ -f "$PROJECT_ROOT/docs/doxyfile.in" ]] || [[ -f "$PROJECT_ROOT/Doxyfile" ]]; then
        # Use existing Doxyfile or generate one
        if [[ -f "$PROJECT_ROOT/Doxyfile" ]]; then
            doxygen "$PROJECT_ROOT/Doxyfile"
        else
            log_warning "Doxyfile not found. Creating basic configuration..."
            # Create a basic Doxyfile
            cat > "$BUILD_DIR/Doxyfile" <<EOF
PROJECT_NAME           = "VneLogging"
PROJECT_NUMBER         = "1.0.0"
INPUT                  = $PROJECT_ROOT/include $PROJECT_ROOT/src
FILE_PATTERNS          = *.h *.hpp *.cpp
RECURSIVE              = YES
OUTPUT_DIRECTORY       = $DOXYGEN_OUTPUT
GENERATE_HTML          = YES
GENERATE_LATEX         = NO
EOF
            doxygen "$BUILD_DIR/Doxyfile"
        fi

        # Copy generated docs to docs directory
        if [[ -d "$BUILD_DIR/html" ]]; then
            mkdir -p "$DOXYGEN_OUTPUT"
            cp -r "$BUILD_DIR/html"/* "$DOXYGEN_OUTPUT/"
            log_success "API documentation generated successfully"
        else
            log_warning "API documentation generation failed"
        fi
    else
        log_warning "Doxyfile configuration not found. Skipping API docs."
    fi

    cd "$PROJECT_ROOT"
}

# Validate documentation links
validate_links() {
    if [[ "$LINK_CHECK_AVAILABLE" == "false" ]]; then
        log_warning "Skipping link validation (markdown-link-check not available)"
        return
    fi

    log_info "Validating documentation links..."

    local failed_links=0

    # Find all markdown files and check links
    if [[ -d "$DOCS_DIR" ]]; then
        while IFS= read -r -d '' file; do
            log_info "Checking links in: $file"
            if ! markdown-link-check "$file"; then
                ((failed_links++))
            fi
        done < <(find "$DOCS_DIR" -name "*.md" -print0 2>/dev/null || true)
    fi

    # Also check README
    if [[ -f "$PROJECT_ROOT/README.md" ]]; then
        log_info "Checking links in: README.md"
        if ! markdown-link-check "$PROJECT_ROOT/README.md"; then
            ((failed_links++))
        fi
    fi

    if [[ $failed_links -eq 0 ]]; then
        log_success "All documentation links are valid"
    else
        log_error "Found $failed_links files with broken links"
        return 1
    fi
}

# Generate documentation coverage report
generate_coverage_report() {
    log_info "Generating documentation coverage report..."

    local total_files=0
    local documented_files=0

    # Count source files
    if [[ -d "$PROJECT_ROOT/src" ]]; then
        while IFS= read -r -d '' file; do
            ((total_files++))
        done < <(find "$PROJECT_ROOT/src" -name "*.cpp" -o -name "*.h" -print0 2>/dev/null || true)
    fi

    # Count documented files (simplified check - files with doxygen comments)
    if [[ -d "$PROJECT_ROOT/src" ]]; then
        while IFS= read -r -d '' file; do
            if grep -q "@brief\|@class\|@function\|@param" "$file" 2>/dev/null; then
                ((documented_files++))
            fi
        done < <(find "$PROJECT_ROOT/src" -name "*.cpp" -o -name "*.h" -print0 2>/dev/null || true)
    fi

    if [[ $total_files -gt 0 ]]; then
        local coverage=$((documented_files * 100 / total_files))
        log_info "Documentation Coverage: $documented_files/$total_files files ($coverage%)"

        if [[ $coverage -lt 80 ]]; then
            log_warning "Documentation coverage is below 80%"
            return 1
        else
            log_success "Documentation coverage is acceptable"
        fi
    else
        log_warning "No source files found to analyze"
    fi
}

# Main function
main() {
    log_info "Starting VneLogging documentation generation..."

    # Check prerequisites
    check_prerequisites

    # Generate API documentation
    generate_api_docs

    # Validate documentation
    if [[ -d "$DOCS_DIR" ]]; then
        validate_links
    fi

    # Generate coverage report
    generate_coverage_report

    log_success "Documentation generation completed successfully!"
    if [[ "$DOXYGEN_AVAILABLE" == "true" && -d "$DOXYGEN_OUTPUT" ]]; then
        log_info "API documentation available at: $DOXYGEN_OUTPUT/index.html"
    fi
}

# Help function
show_help() {
    echo "VneLogging Documentation Generator"
    echo ""
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  --help, -h     Show this help message"
    echo "  --api-only     Generate only API documentation"
    echo "  --validate     Only validate existing documentation"
    echo ""
    echo "Examples:"
    echo "  $0              # Generate all documentation"
    echo "  $0 --api-only   # Generate only API docs"
    echo "  $0 --validate   # Only validate existing docs"
}

# Parse command line arguments
case "${1:-}" in
    --help|-h)
        show_help
        exit 0
        ;;
    --api-only)
        check_prerequisites
        generate_api_docs
        ;;
    --validate)
        check_prerequisites
        validate_links
        generate_coverage_report
        ;;
    "")
        main
        ;;
    *)
        log_error "Unknown option: $1"
        show_help
        exit 1
        ;;
esac
