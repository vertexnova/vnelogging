#!/usr/bin/env python3
"""
VneLogging Cppcheck Analyzer

A script to run cppcheck static analysis on C/C++ source files with
VneLogging-specific configuration and filter out third-party warnings.

Usage:
    python scripts/cppcheck_analyzer.py <folder_path> [options]
    python scripts/cppcheck_analyzer.py --file <file_path> [options]

Examples:
    python scripts/cppcheck_analyzer.py src/vnelogging
    python scripts/cppcheck_analyzer.py src/vnelogging --verbose
    python scripts/cppcheck_analyzer.py --file src/vertexnova/logging/logging.cpp
"""

import argparse
import os
import subprocess
import sys
import xml.etree.ElementTree as ET
from pathlib import Path
from datetime import datetime


def find_source_files(folder_path: Path) -> list:
    """Find all C/C++ source files in the specified folder."""
    source_files = []

    # Supported file extensions
    extensions = ('.h', '.cpp', '.mm', '.m', '.hpp', '.c')

    # Directories to exclude
    exclude_dirs = {'build', '.git', 'node_modules', 'CMakeFiles', '__pycache__'}

    for root, dirs, files in os.walk(folder_path):
        # Remove excluded directories from dirs list to prevent walking into them
        dirs[:] = [d for d in dirs if d not in exclude_dirs]

        for file in files:
            if file.endswith(extensions):
                source_files.append(os.path.join(root, file))

    return source_files


def is_source_file(file_path: Path) -> bool:
    """Check if a file is a supported source file."""
    extensions = ('.h', '.cpp', '.mm', '.m', '.hpp', '.c')
    return file_path.suffix.lower() in extensions


def check_cppcheck() -> bool:
    """Check if cppcheck is available."""
    try:
        subprocess.run(['cppcheck', '--version'],
                      capture_output=True,
                      check=True)
        return True
    except (subprocess.CalledProcessError, FileNotFoundError):
        return False


def generate_html_report(xml_file: Path, output_dir: Path, folder_name: str) -> int:
    """Generate a readable HTML report from the XML file."""
    try:
        tree = ET.parse(xml_file)
        root = tree.getroot()

        # Find all error elements
        errors = root.findall('.//error')
        filtered_errors = []

        script_dir = Path(__file__).parent
        project_root = script_dir.parent

        for error in errors:
            # Get the file path from the error
            file_elem = error.find('location')
            if file_elem is not None:
                file_path = file_elem.get('file', '')

                # Skip if it's a third-party file
                if any(third_party in file_path for third_party in [
                    '3rd_party', 'third_party', 'external', 'vendor',
                    'googletest', 'gtest', 'gmock'
                ]):
                    continue

                filtered_errors.append(error)

        # Create HTML report
        html_content = f"""<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Cppcheck Report - {folder_name}</title>
    <style>
        body {{ font-family: Arial, sans-serif; margin: 20px; background-color: #f5f5f5; }}
        .header {{ background-color: #2c3e50; color: white; padding: 20px; border-radius: 5px; margin-bottom: 20px; }}
        .summary {{ background-color: white; padding: 15px; border-radius: 5px; margin-bottom: 20px; }}
        .error {{ background-color: white; padding: 15px; border-radius: 5px; margin-bottom: 10px; border-left: 4px solid #e74c3c; }}
        .error.warning {{ border-left-color: #f39c12; }}
        .error.info {{ border-left-color: #3498db; }}
        .error.performance {{ border-left-color: #9b59b6; }}
        .error.portability {{ border-left-color: #1abc9c; }}
        .error.style {{ border-left-color: #34495e; }}
        .file-path {{ font-weight: bold; color: #2c3e50; }}
        .line-number {{ color: #7f8c8d; }}
        .severity {{ font-weight: bold; }}
        .severity.error {{ color: #e74c3c; }}
        .severity.warning {{ color: #f39c12; }}
        .severity.info {{ color: #3498db; }}
        .severity.performance {{ color: #9b59b6; }}
        .severity.portability {{ color: #1abc9c; }}
        .severity.style {{ color: #34495e; }}
        .message {{ margin-top: 10px; }}
        .timestamp {{ color: #7f8c8d; font-size: 0.9em; }}
    </style>
</head>
<body>
    <div class="header">
        <h1>Cppcheck Analysis Report</h1>
        <p>Target: {folder_name}</p>
        <p class="timestamp">Generated: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}</p>
    </div>

    <div class="summary">
        <h2>Summary</h2>
        <p>Total issues found: {len(filtered_errors)}</p>
        <p>Third-party warnings have been filtered out.</p>
    </div>
"""

        if filtered_errors:
            html_content += '<h2>Issues Found</h2>'

            for error in filtered_errors:
                severity = error.get('severity', 'unknown')
                msg = error.get('msg', 'No message')
                id_attr = error.get('id', 'unknown')

                # Get file and line information
                location = error.find('location')
                file_path = location.get('file', 'Unknown file') if location is not None else 'Unknown file'
                line = location.get('line', 'Unknown line') if location is not None else 'Unknown line'

                # Clean up file path for display
                try:
                    display_path = str(Path(file_path).relative_to(project_root))
                except ValueError:
                    display_path = file_path

                html_content += f"""
    <div class="error {severity}">
        <div class="file-path">{display_path}</div>
        <div class="line-number">Line: {line}</div>
        <div class="severity {severity}">[{severity.upper()}] {id_attr}</div>
        <div class="message">{msg}</div>
    </div>
"""
        else:
            html_content += """
    <div class="summary">
        <h2>✅ No Issues Found</h2>
        <p>Great! No static analysis issues were detected in the VneLogging code.</p>
    </div>
"""

        html_content += """
</body>
</html>
"""

        # Write HTML report
        html_file = output_dir / 'cppcheck-report.html'
        with open(html_file, 'w', encoding='utf-8') as f:
            f.write(html_content)

        return len(filtered_errors)

    except ET.ParseError as e:
        print(f"Error parsing XML report: {e}")
        return 0


def generate_file_report(xml_file: Path, output_dir: Path, file_path: str, folder_name: str) -> int:
    """Generate a detailed HTML report for a specific file."""
    try:
        tree = ET.parse(xml_file)
        root = tree.getroot()

        # Find all error elements for this specific file
        errors = root.findall('.//error')
        file_errors = []

        for error in errors:
            # Get the file path from the error
            file_elem = error.find('location')
            if file_elem is not None:
                error_file_path = file_elem.get('file', '')

                # Only include errors for this specific file
                if error_file_path == file_path:
                    # Skip if it's a third-party file
                    if any(third_party in error_file_path for third_party in [
                        '3rd_party', 'third_party', 'external', 'vendor',
                        'googletest', 'gtest', 'gmock'
                    ]):
                        continue

                    file_errors.append(error)

        # Create HTML report for this file
        file_name = Path(file_path).name
        script_dir = Path(__file__).parent
        project_root = script_dir.parent
        display_path = str(Path(file_path).relative_to(project_root))

        html_content = f"""<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Cppcheck Report - {file_name}</title>
    <style>
        body {{ font-family: Arial, sans-serif; margin: 20px; background-color: #f5f5f5; }}
        .header {{ background-color: #2c3e50; color: white; padding: 20px; border-radius: 5px; margin-bottom: 20px; }}
        .summary {{ background-color: white; padding: 15px; border-radius: 5px; margin-bottom: 20px; }}
        .error {{ background-color: white; padding: 15px; border-radius: 5px; margin-bottom: 10px; border-left: 4px solid #e74c3c; }}
        .error.warning {{ border-left-color: #f39c12; }}
        .error.info {{ border-left-color: #3498db; }}
        .error.performance {{ border-left-color: #9b59b6; }}
        .error.portability {{ border-left-color: #1abc9c; }}
        .error.style {{ border-left-color: #34495e; }}
        .file-path {{ font-weight: bold; color: #2c3e50; }}
        .line-number {{ color: #7f8c8d; }}
        .severity {{ font-weight: bold; }}
        .severity.error {{ color: #e74c3c; }}
        .severity.warning {{ color: #f39c12; }}
        .severity.info {{ color: #3498db; }}
        .severity.performance {{ color: #9b59b6; }}
        .severity.portability {{ color: #1abc9c; }}
        .severity.style {{ color: #34495e; }}
        .message {{ margin-top: 10px; }}
        .timestamp {{ color: #7f8c8d; font-size: 0.9em; }}
        .back-link {{ margin-bottom: 20px; }}
        .back-link a {{ color: #3498db; text-decoration: none; }}
        .back-link a:hover {{ text-decoration: underline; }}
    </style>
</head>
<body>
    <div class="back-link">
        <a href="index.html">← Back to Index</a>
    </div>

    <div class="header">
        <h1>Cppcheck Analysis Report</h1>
        <p>File: {display_path}</p>
        <p class="timestamp">Generated: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}</p>
    </div>

    <div class="summary">
        <h2>Summary</h2>
        <p>Total issues found: {len(file_errors)}</p>
        <p>Third-party warnings have been filtered out.</p>
    </div>
"""

        if file_errors:
            html_content += '<h2>Issues Found</h2>'

            for error in file_errors:
                severity = error.get('severity', 'unknown')
                msg = error.get('msg', 'No message')
                id_attr = error.get('id', 'unknown')

                # Get file and line information
                location = error.find('location')
                line = location.get('line', 'Unknown line') if location is not None else 'Unknown line'

                html_content += f"""
    <div class="error {severity}">
        <div class="line-number">Line: {line}</div>
        <div class="severity {severity}">[{severity.upper()}] {id_attr}</div>
        <div class="message">{msg}</div>
    </div>
"""
        else:
            html_content += """
    <div class="summary">
        <h2>✅ No Issues Found</h2>
        <p>Great! No static analysis issues were detected in this file.</p>
    </div>
"""

        html_content += """
</body>
</html>
"""

        # Write HTML report
        safe_filename = Path(file_path).name.replace('.', '_').replace('/', '_')
        html_file = output_dir / f'{safe_filename}-cppcheck-report.html'
        with open(html_file, 'w', encoding='utf-8') as f:
            f.write(html_content)

        return len(file_errors)

    except ET.ParseError as e:
        print(f"Error parsing XML report: {e}")
        return 0


def generate_index_html(report_dir: Path, folder_name: str, file_reports: list) -> None:
    """Generate an index.html file that provides an overview of all reports."""
    html_content = f"""<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Static Analysis Reports - {folder_name}</title>
    <style>
        body {{ font-family: Arial, sans-serif; margin: 20px; background-color: #f5f5f5; }}
        .header {{ background-color: #2c3e50; color: white; padding: 20px; border-radius: 5px; margin-bottom: 20px; }}
        .summary {{ background-color: white; padding: 15px; border-radius: 5px; margin-bottom: 20px; }}
        .report-section {{ background-color: white; padding: 15px; border-radius: 5px; margin-bottom: 15px; }}
        .report-link {{ color: #3498db; text-decoration: none; font-weight: bold; }}
        .report-link:hover {{ text-decoration: underline; }}
        .timestamp {{ color: #7f8c8d; font-size: 0.9em; }}
        .status {{ padding: 5px 10px; border-radius: 3px; color: white; font-size: 0.9em; }}
        .status.success {{ background-color: #27ae60; }}
        .status.warning {{ background-color: #f39c12; }}
        .status.error {{ background-color: #e74c3c; }}
        .file-info {{ display: flex; justify-content: space-between; align-items: center; }}
        .file-details {{ flex-grow: 1; }}
    </style>
</head>
<body>
    <div class="header">
        <h1>Static Analysis Reports</h1>
        <p>Target: {folder_name}</p>
        <p class="timestamp">Generated: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}</p>
    </div>

    <div class="summary">
        <h2>Summary</h2>
        <p>Total files analyzed: {len(file_reports)}</p>
        <p>Reports are filtered to exclude third-party warnings.</p>
    </div>
"""

    script_dir = Path(__file__).parent
    project_root = script_dir.parent

    for report in file_reports:
        file_name = report['file']
        issue_count = report['issues']
        report_path = report['path']

        # Determine status
        if issue_count == 0:
            status = 'success'
            status_text = '✅ Clean'
        elif issue_count <= 5:
            status = 'warning'
            status_text = f'⚠️ {issue_count} issues'
        else:
            status = 'error'
            status_text = f'❌ {issue_count} issues'

        # Clean up file path for display
        try:
            display_path = str(Path(file_name).relative_to(project_root))
        except ValueError:
            display_path = file_name

        html_content += f"""
    <div class="report-section">
        <div class="file-info">
            <div class="file-details">
                <div class="report-link"><a href="{report_path}" target="_blank">{display_path}</a></div>
                <div class="timestamp">Issues: {issue_count}</div>
            </div>
            <div class="status {status}">{status_text}</div>
        </div>
    </div>
"""

    html_content += """
</body>
</html>
"""

    # Write index file
    index_file = report_dir / 'index.html'
    with open(index_file, 'w', encoding='utf-8') as f:
        f.write(html_content)


def run_cppcheck(files: list, folder_name: str, verbose: bool = False, save_raw_xml: bool = False) -> bool:
    """Run cppcheck on the specified files."""
    if not files:
        print("No source files found to analyze.")
        return True

    print(f"Found {len(files)} source files to analyze.")

    # Create report directory structure
    script_dir = Path(__file__).parent
    project_root = script_dir.parent
    build_dir = project_root / 'build'
    build_dir.mkdir(exist_ok=True)

    # Create directory based on folder path
    report_dir = build_dir / 'cppcheck-reports' / folder_name
    report_dir.mkdir(parents=True, exist_ok=True)

    # Define report files
    raw_report = report_dir / 'cppcheck-raw.xml'

    # Build cppcheck command
    cmd = [
        'cppcheck',
        '--enable=all',                    # Enable all checks
        '--enable=style',                  # Enable style checks
        '--std=c++20',                     # C++20 standard
        '--language=c++',                  # C++ language
        '--inline-suppr',                  # Enable inline suppressions
        '--inconclusive',                  # Include inconclusive results
        '--quiet',                         # Suppress progress messages
        '--suppress=missingIncludeSystem', # Suppress missing system include warnings
        '--suppress=unusedFunction',       # Suppress unused function warnings
        '--suppress=unmatchedSuppression', # Suppress unmatched suppression warnings
        '--xml',                           # XML output
        '--xml-version=2',                 # XML version 2
        f'--output-file={raw_report}'      # Output to raw report file
    ]

    # Add include paths for VneLogging
    cmd.extend([
        '-I', str(project_root / 'include'),
        '-I', str(project_root / 'src'),
    ])

    # Add all files
    cmd.extend(files)

    if verbose:
        print(f"Running command: {' '.join(cmd)}")
        print()

    try:
        result = subprocess.run(cmd,
                              capture_output=True,
                              text=True,
                              check=False)

        # Print stderr (cppcheck errors/warnings)
        if result.stderr:
            print("Cppcheck Errors/Warnings:")
            print(result.stderr)

        # Generate individual file reports
        if raw_report.exists():
            print(f"\nGenerating individual file reports from {raw_report}...")

            file_reports = []
            total_issues = 0

            for file_path in files:
                issue_count = generate_file_report(raw_report, report_dir, file_path, folder_name)
                total_issues += issue_count

                # Create report entry for index
                safe_filename = Path(file_path).name.replace('.', '_').replace('/', '_')
                report_path = f'{safe_filename}-cppcheck-report.html'

                file_reports.append({
                    'file': file_path,
                    'issues': issue_count,
                    'path': report_path
                })

                if verbose:
                    print(f"  {Path(file_path).name}: {issue_count} issues")

            # Generate index.html
            generate_index_html(report_dir, folder_name, file_reports)
            print(f"Index page saved to {report_dir}/index.html")
            print(f"Found {total_issues} total issues in VneLogging code (excluding third-party)")

            # Clean up raw report unless requested to save it
            if not save_raw_xml:
                raw_report.unlink()
                print("Raw XML report cleaned up (use --save-raw-xml to keep it)")
            else:
                print(f"Raw XML report saved to {raw_report}")
        else:
            print("No XML report generated. Check cppcheck output above.")
            return False

        # Return success if no errors found
        return result.returncode == 0

    except FileNotFoundError:
        print("✗ cppcheck not found. Please install cppcheck.")
        return False


def main():
    """Main function."""
    parser = argparse.ArgumentParser(
        description="Run cppcheck static analysis on C/C++ source files",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python scripts/cppcheck_analyzer.py src/vnelogging
  python scripts/cppcheck_analyzer.py src/vnelogging --verbose
  python scripts/cppcheck_analyzer.py --file src/vertexnova/logging/logging.cpp
  python scripts/cppcheck_analyzer.py tests
  python scripts/cppcheck_analyzer.py src/vnelogging --save-raw-xml
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
        '--verbose',
        action='store_true',
        help='Show verbose output including command details'
    )
    parser.add_argument(
        '--save-raw-xml',
        action='store_true',
        help='Save the raw XML report (normally deleted after processing)'
    )

    args = parser.parse_args()

    # Get the project root directory
    script_dir = Path(__file__).parent
    project_root = script_dir.parent

    print("VneLogging Cppcheck Analyzer")
    print("=" * 40)
    print()

    if args.file:
        # Analyze specific file
        target_file = project_root / args.file

        if not target_file.exists():
            print(f"Error: File not found at {target_file}")
            sys.exit(1)

        if not is_source_file(target_file):
            print(f"Error: {target_file} is not a supported source file type.")
            print("Supported extensions: .h, .cpp, .mm, .m, .hpp, .c")
            sys.exit(1)

        print(f"Target file: {target_file}")
        print()

        source_files = [str(target_file)]
        folder_name = target_file.parent.name

    else:
        # Analyze folder
        target_folder = project_root / args.folder

        if not target_folder.exists():
            print(f"Error: Target folder not found at {target_folder}")
            sys.exit(1)

        print(f"Target folder: {target_folder}")
        print()

        # Find source files
        source_files = find_source_files(target_folder)
        folder_name = args.folder

    if args.verbose:
        print("Source files found:")
        for file in source_files:
            print(f"  {file}")
        print()

    # Run cppcheck
    success = run_cppcheck(source_files, folder_name, args.verbose, args.save_raw_xml)

    if success:
        print("\n✓ Cppcheck analysis completed successfully!")
        print(f"HTML reports saved to build/cppcheck-reports/{folder_name}/")
    else:
        print("\n✗ Cppcheck analysis found issues!")
        print("Please review the output above and fix any issues.")
        sys.exit(1)


if __name__ == "__main__":
    main()
