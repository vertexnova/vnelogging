#!/usr/bin/env python3
"""
VneLogging Static Analyzer

A combined script to run both cppcheck and clang-tidy static analysis
on C/C++ source files with VneLogging-specific configuration.

Usage:
    python scripts/static_analyzer.py <folder_path> [options]
    python scripts/static_analyzer.py --file <file_path> [options]

Examples:
    python scripts/static_analyzer.py src/vnelogging
    python scripts/static_analyzer.py src/vnelogging --cppcheck-only
    python scripts/static_analyzer.py src/vnelogging --clang-tidy-only
    python scripts/static_analyzer.py --file src/vertexnova/logging/logging.cpp
"""

import argparse
import subprocess
import sys
from pathlib import Path


def main():
    """Main function."""
    parser = argparse.ArgumentParser(
        description="Run static analysis with cppcheck and/or clang-tidy",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python scripts/static_analyzer.py src/vnelogging
  python scripts/static_analyzer.py src/vnelogging --cppcheck-only
  python scripts/static_analyzer.py src/vnelogging --clang-tidy-only
  python scripts/static_analyzer.py --file src/vertexnova/logging/logging.cpp
  python scripts/static_analyzer.py tests
  python scripts/static_analyzer.py src/vnelogging --save-raw-xml --save-raw-json
        """
    )

    # Create mutually exclusive group for folder vs file
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument(
        'folder',
        nargs='?',
        help='Folder path to analyze (e.g., src/vnelogging)'
    )
    group.add_argument(
        '--file',
        help='Specific file to analyze (e.g., src/vertexnova/logging/logging.cpp)'
    )

    parser.add_argument(
        '--cppcheck-only',
        action='store_true',
        help='Run only cppcheck analysis'
    )
    parser.add_argument(
        '--clang-tidy-only',
        action='store_true',
        help='Run only clang-tidy analysis'
    )
    parser.add_argument(
        '--verbose',
        action='store_true',
        help='Show verbose output including command details'
    )
    parser.add_argument(
        '--save-raw-xml',
        action='store_true',
        help='Save the raw XML report from cppcheck (normally deleted after processing)'
    )
    parser.add_argument(
        '--save-raw-json',
        action='store_true',
        help='Save the raw JSON report from clang-tidy (normally deleted after processing)'
    )

    args = parser.parse_args()

    # Get the project root directory
    script_dir = Path(__file__).parent
    project_root = script_dir.parent

    print("VneLogging Static Analyzer")
    print("=" * 40)
    print()

    # Run analyses
    success = True

    # Run cppcheck
    if not args.clang_tidy_only:
        print("🔍 Running Cppcheck Analysis...")
        print("-" * 30)

        cmd = [sys.executable, str(script_dir / "cppcheck_analyzer.py")]
        if args.verbose:
            cmd.append("--verbose")
        if args.save_raw_xml:
            cmd.append("--save-raw-xml")

        # Add target arguments properly
        if args.file:
            cmd.extend(["--file", args.file])
        else:
            cmd.append(args.folder)

        result = subprocess.run(cmd, check=False)
        if result.returncode != 0:
            success = False
            print("❌ Cppcheck analysis failed!")
        else:
            print("✅ Cppcheck analysis completed!")
        print()

    # Run clang-tidy
    if not args.cppcheck_only:
        print("🔍 Running Clang-Tidy Analysis...")
        print("-" * 30)

        cmd = [sys.executable, str(script_dir / "clang_tidy_analyzer.py")]
        if args.verbose:
            cmd.append("--verbose")
        if args.save_raw_json:
            cmd.append("--save-raw-json")

        # Add target arguments properly
        if args.file:
            cmd.extend(["--file", args.file])
        else:
            cmd.append(args.folder)

        result = subprocess.run(cmd, check=False)
        if result.returncode != 0:
            success = False
            print("❌ Clang-Tidy analysis failed!")
        else:
            print("✅ Clang-Tidy analysis completed!")
        print()

    # Summary
    print("📊 Analysis Summary")
    print("=" * 40)

    if args.cppcheck_only:
        print("✅ Cppcheck analysis completed")
        print("📁 Reports: build/cppcheck-reports/")
    elif args.clang_tidy_only:
        print("✅ Clang-Tidy analysis completed")
        print("📁 Reports: build/clang-tidy-reports/")
    else:
        print("✅ Both analyses completed")
        print("📁 Cppcheck Reports: build/cppcheck-reports/")
        print("📁 Clang-Tidy Reports: build/clang-tidy-reports/")

    print("\n🌐 Open index.html files in your browser to view detailed reports!")

    if not success:
        sys.exit(1)


if __name__ == "__main__":
    main()
