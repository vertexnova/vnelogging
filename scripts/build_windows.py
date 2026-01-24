#!/usr/bin/env python3
"""
VneLogging Windows Build Script
Copyright (c) 2024 Ajeet Singh Yadav. All rights reserved.
Licensed under the Apache License, Version 2.0 (the "License")

This script builds VneLogging for Windows with Visual Studio integration.
"""

import os
import sys
import subprocess
import argparse
import platform
import shutil
import re
from pathlib import Path
from typing import List, Optional


class BuildConfig:
    """Configuration for the build process."""

    def __init__(self):
        self.jobs = 10
        self.build_type = "Debug"
        self.action = "configure_and_build"
        self.clean_build = False
        self.interactive = False
        self.platform = "Windows"
        self.compiler = "cl"


def find_visual_studio() -> Optional[str]:
    """Find Visual Studio installation."""
    vs_paths = [
        r"C:\Program Files\Microsoft Visual Studio\2022\Community",
        r"C:\Program Files\Microsoft Visual Studio\2022\Professional",
        r"C:\Program Files\Microsoft Visual Studio\2022\Enterprise",
        r"C:\Program Files (x86)\Microsoft Visual Studio\2022\Community",
        r"C:\Program Files (x86)\Microsoft Visual Studio\2022\Professional",
        r"C:\Program Files (x86)\Microsoft Visual Studio\2022\Enterprise",
        r"C:\Program Files\Microsoft Visual Studio\2019\Community",
        r"C:\Program Files\Microsoft Visual Studio\2019\Professional",
        r"C:\Program Files\Microsoft Visual Studio\2019\Enterprise",
        r"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community",
        r"C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional",
        r"C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise",
    ]

    for vs_path in vs_paths:
        if os.path.exists(vs_path):
            return vs_path
    return None


def get_compiler_version() -> str:
    """Get MSVC compiler version."""
    try:
        result = subprocess.run(
            ["cl"], capture_output=True, text=True, stderr=subprocess.STDOUT
        )
        output = result.stdout
        # Try to extract version from output
        match = re.search(r"Version (\d+\.\d+\.\d+)", output)
        if match:
            return match.group(1)
    except (subprocess.CalledProcessError, FileNotFoundError):
        pass

    # Fallback: detect from Visual Studio installation
    vs_path = find_visual_studio()
    if vs_path:
        if "2022" in vs_path:
            return "2022"
        elif "2019" in vs_path:
            return "2019"

    return "unknown"


def check_visual_studio_env() -> bool:
    """Check if Visual Studio environment is set up."""
    try:
        subprocess.run(["cl"], capture_output=True, check=False)
        return True
    except FileNotFoundError:
        return False


def setup_visual_studio_env():
    """Setup Visual Studio environment."""
    vs_path = find_visual_studio()
    if not vs_path:
        print("Error: Visual Studio not found")
        print("Please install Visual Studio 2019 or 2022")
        sys.exit(1)

    vsdevcmd = os.path.join(
        vs_path, "Common7", "Tools", "VsDevCmd.bat"
    )
    if not os.path.exists(vsdevcmd):
        print(f"Error: VsDevCmd.bat not found at {vsdevcmd}")
        sys.exit(1)

    print(f"Found Visual Studio at: {vs_path}")
    print("Please run this script from a Visual Studio Developer Command Prompt")
    print(f"Or run: {vsdevcmd}")
    print("Then try again.")
    sys.exit(1)


def check_cmake() -> bool:
    """Check if CMake is available."""
    try:
        subprocess.run(["cmake", "--version"], capture_output=True, check=True)
        return True
    except (subprocess.CalledProcessError, FileNotFoundError):
        return False


def interactive_mode(config: BuildConfig):
    """Run interactive configuration mode."""
    print("=== VneLogging Interactive Build Configuration ===")
    print()
    print(f"Detected Platform: {config.platform}")
    print()

    # Build type selection
    print("Select Build Type:")
    print("1) Debug (default) - Development with debug symbols")
    print("2) Release - Optimized for production")
    print("3) RelWithDebInfo - Release with debug info")
    print("4) MinSizeRel - Minimum size release")
    build_choice = input("Enter choice (1-4) [1]: ").strip()
    if build_choice == "2":
        config.build_type = "Release"
    elif build_choice == "3":
        config.build_type = "RelWithDebInfo"
    elif build_choice == "4":
        config.build_type = "MinSizeRel"
    else:
        config.build_type = "Debug"

    # Action selection
    print()
    print("Select Action:")
    print("1) Configure only")
    print("2) Configure and build (default)")
    print("3) Configure, build, and test")
    action_choice = input("Enter choice (1-3) [2]: ").strip()
    if action_choice == "1":
        config.action = "configure"
    elif action_choice == "3":
        config.action = "test"
    else:
        config.action = "configure_and_build"

    # Clean build option
    print()
    clean_choice = input("Clean build directory before starting? (y/N): ").strip()
    if clean_choice.lower() == "y":
        config.clean_build = True

    # Jobs
    print()
    jobs_input = input(f"Number of parallel jobs (default: {config.jobs}): ").strip()
    if jobs_input:
        try:
            config.jobs = int(jobs_input)
        except ValueError:
            print(f"Invalid input, using default: {config.jobs}")

    print()
    print("=== Configuration Summary ===")
    print(f"Platform: {config.platform}")
    print(f"Build Type: {config.build_type}")
    print(f"Compiler: {config.compiler}")
    print(f"Action: {config.action}")
    print(f"Clean Build: {config.clean_build}")
    print(f"Parallel Jobs: {config.jobs}")
    print()
    proceed = input("Proceed with this configuration? (Y/n): ").strip()
    if proceed.lower() == "n":
        print("Build cancelled.")
        sys.exit(0)


def build_cmake_command(project_root: Path, build_type: str) -> List[str]:
    """Build CMake configuration command."""
    args = [
        "cmake",
        "-G", "Visual Studio 17 2022",
        "-A", "x64",
        "-DCMAKE_BUILD_TYPE=" + build_type,
        "-DCMAKE_C_COMPILER=cl",
        "-DCMAKE_CXX_COMPILER=cl",
        "-DBUILD_TESTS=ON",
        str(project_root),
    ]
    return args


def clean_build_dir(build_dir: Path):
    """Clean the build directory."""
    if build_dir.exists():
        print(f"Cleaning build directory: {build_dir}")
        shutil.rmtree(build_dir)
    build_dir.mkdir(parents=True, exist_ok=True)


def ensure_build_dir(build_dir: Path):
    """Ensure build directory exists."""
    build_dir.mkdir(parents=True, exist_ok=True)


def configure_project(build_dir: Path, project_root: Path, build_type: str) -> bool:
    """Configure the project with CMake."""
    print("Configuring project...")
    cmake_args = build_cmake_command(project_root, build_type)
    print(f"Running: {' '.join(cmake_args)}")

    try:
        result = subprocess.run(
            cmake_args,
            cwd=build_dir,
            check=True,
        )
        return True
    except subprocess.CalledProcessError as e:
        print("Error: CMake configuration failed")
        print("Please check the error messages above and ensure:")
        print("1. Visual Studio 2022 is installed")
        print("2. CMake is installed and in PATH")
        print("3. All required dependencies are available")
        return False


def build_project(build_dir: Path, build_type: str, jobs: int) -> bool:
    """Build the project."""
    print(f"Building with {jobs} parallel jobs...")
    build_args = [
        "cmake",
        "--build", ".",
        "--config", build_type,
        "--parallel", str(jobs),
    ]

    try:
        subprocess.run(build_args, cwd=build_dir, check=True)
        return True
    except subprocess.CalledProcessError as e:
        print("Error: Build failed")
        print("Please check the error messages above")
        return False


def run_tests(build_dir: Path, build_type: str) -> bool:
    """Run tests."""
    print("Running tests...")
    test_args = [
        "ctest",
        "-C", build_type,
        "--output-on-failure",
    ]

    try:
        subprocess.run(test_args, cwd=build_dir, check=True)
        return True
    except subprocess.CalledProcessError as e:
        print("Warning: Some tests failed")
        print("Check the test output above for details")
        return False


def main():
    """Main function."""
    parser = argparse.ArgumentParser(
        description="Build VneLogging for Windows",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python build_windows.py -t Debug -a configure_and_build
  python build_windows.py --build-type Release --jobs 8
  python build_windows.py --interactive
  python build_windows.py --build-type Release --clean
        """,
    )

    parser.add_argument(
        "-t", "--build-type",
        choices=["Debug", "Release", "RelWithDebInfo", "MinSizeRel"],
        default="Debug",
        help="Build type (default: Debug)",
    )
    parser.add_argument(
        "-a", "--action",
        choices=["configure", "build", "configure_and_build", "test"],
        default="configure_and_build",
        help="Action to perform (default: configure_and_build)",
    )
    parser.add_argument(
        "-j", "--jobs",
        type=int,
        default=10,
        help="Number of parallel jobs (default: 10)",
    )
    parser.add_argument(
        "--clean",
        action="store_true",
        help="Clean build directory before building",
    )
    parser.add_argument(
        "--interactive",
        action="store_true",
        help="Run in interactive mode",
    )

    args = parser.parse_args()

    config = BuildConfig()
    config.build_type = args.build_type
    config.action = args.action
    config.jobs = args.jobs
    config.clean_build = args.clean
    config.interactive = args.interactive

    # Run interactive mode if requested
    if config.interactive:
        interactive_mode(config)

    # Check Visual Studio environment
    if not check_visual_studio_env():
        setup_visual_studio_env()

    # Check CMake
    if not check_cmake():
        print("Error: CMake not found in PATH")
        print("Please install CMake and ensure it's in your PATH")
        print("Download from: https://cmake.org/download/")
        sys.exit(1)

    # Get compiler version
    compiler_version = get_compiler_version()
    print(f"{config.platform} :: {config.compiler}-{compiler_version}")

    # Get project root
    script_dir = Path(__file__).parent
    project_root = script_dir.parent

    # Set build directory
    build_dir = (
        project_root
        / "build"
        / config.build_type
        / f"build-windows-{config.compiler}-{compiler_version}"
    )

    # Clean or ensure build directory
    if config.clean_build:
        clean_build_dir(build_dir)
    else:
        ensure_build_dir(build_dir)

    # Perform action
    success = True
    if config.action in ["configure", "build", "configure_and_build", "test"]:
        if not configure_project(build_dir, project_root, config.build_type):
            sys.exit(1)

    if config.action in ["build", "configure_and_build", "test"]:
        if not build_project(build_dir, config.build_type, config.jobs):
            sys.exit(1)

    if config.action == "test":
        run_tests(build_dir, config.build_type)

    print()
    print("=== Build completed successfully ===")
    print(f"Build directory: {build_dir}")


if __name__ == "__main__":
    main()
