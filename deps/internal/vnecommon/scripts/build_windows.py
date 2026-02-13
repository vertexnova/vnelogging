#!/usr/bin/env python3
# ----------------------------------------------------------------------
# Copyright (c) 2024 Ajeet Singh Yadav. All rights reserved.
# Licensed under the Apache License, Version 2.0 (the "License")
#
# Author:    Ajeet Singh Yadav
# Created:   January 2025
# ----------------------------------------------------------------------

"""
Windows build script for VneCommon library.

Usage:
    python build_windows.py [options]

Options:
    --release       Build in Release mode
    --debug         Build in Debug mode (default)
    --no-examples   Don't build examples
    --clean         Clean build directory before building
    --run           Run example after building
    --help          Show this help message
"""

import argparse
import os
import shutil
import subprocess
import sys
from pathlib import Path


def get_project_dir() -> Path:
    """Get the project root directory."""
    return Path(__file__).parent.parent.resolve()


def run_command(cmd: list[str], cwd: Path | None = None) -> int:
    """Run a command and return the exit code."""
    print(f"Running: {' '.join(cmd)}")
    result = subprocess.run(cmd, cwd=cwd)
    return result.returncode


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Build VneCommon library on Windows"
    )
    parser.add_argument(
        "--release",
        action="store_true",
        help="Build in Release mode"
    )
    parser.add_argument(
        "--debug",
        action="store_true",
        help="Build in Debug mode (default)"
    )
    parser.add_argument(
        "--no-examples",
        action="store_true",
        help="Don't build examples"
    )
    parser.add_argument(
        "--clean",
        action="store_true",
        help="Clean build directory before building"
    )

    args = parser.parse_args()

    # Determine build type
    build_type = "Release" if args.release else "Debug"
    build_examples = "OFF" if args.no_examples else "ON"

    project_dir = get_project_dir()
    build_dir = project_dir / "build" / build_type

    print("=== VneCommon Build Script (Windows) ===")
    print(f"Build Type: {build_type}")
    print(f"Build Examples: {build_examples}")
    print(f"Build Directory: {build_dir}")
    print()

    # Clean build if requested
    if args.clean and build_dir.exists():
        print("Cleaning build directory...")
        shutil.rmtree(build_dir)

    # Create build directory
    build_dir.mkdir(parents=True, exist_ok=True)

    # Configure
    print("Configuring...")
    configure_cmd = [
        "cmake",
        "-B", str(build_dir),
        "-S", str(project_dir),
        f"-DCMAKE_BUILD_TYPE={build_type}",
        f"-DBUILD_EXAMPLES={build_examples}",
    ]

    if run_command(configure_cmd) != 0:
        print("ERROR: CMake configuration failed")
        return 1

    # Build
    print("Building...")
    build_cmd = [
        "cmake",
        "--build", str(build_dir),
        "--config", build_type,
        "--parallel"
    ]

    if run_command(build_cmd) != 0:
        print("ERROR: Build failed")
        return 1

    print()
    print("=== Build Complete ===")
    print(f"Output: {build_dir / 'bin' / build_type}")

    return 0


if __name__ == "__main__":
    sys.exit(main())
