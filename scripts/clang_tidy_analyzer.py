#!/usr/bin/env python3
"""
VneLogging Clang-Tidy Analyzer

A script to run clang-tidy static analysis on C/C++ source files with
VneLogging-specific configuration and filter out third-party warnings.

Usage:
    python scripts/clang_tidy_analyzer.py <folder_path> [options]
    python scripts/clang_tidy_analyzer.py --file <file_path> [options]

Examples:
    python scripts/clang_tidy_analyzer.py src/vnelogging
    python scripts/clang_tidy_analyzer.py src/vnelogging --verbose
    python scripts/clang_tidy_analyzer.py --file src/vertexnova/logging/logging.cpp
"""

import argparse
import os
import subprocess
import sys
import json
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


def check_clang_tidy() -> bool:
    """Check if clang-tidy is available."""
    try:
        subprocess.run(['clang-tidy', '--version'],
                      capture_output=True,
                      check=True)
        return True
    except (subprocess.CalledProcessError, FileNotFoundError):
        return False


def generate_file_report(json_file: Path, output_dir: Path, file_path: str, folder_name: str) -> int:
    """Generate a detailed HTML report for a specific file."""
    try:
        with open(json_file, 'r', encoding='utf-8') as f:
            data = json.load(f)

        # Find diagnostics for this specific file
        file_diagnostics = []

        for file_data in data:
            current_file_path = file_data.get('file', '')

            # Only include diagnostics for this specific file
            if current_file_path == file_path:
                # Skip if it's a third-party file
                if any(third_party in current_file_path for third_party in [
                    '3rd_party', 'third_party', 'external', 'vendor',
                    'googletest', 'gtest', 'gmock'
                ]):
                    continue

                diagnostics = file_data.get('diagnostics', [])
                file_diagnostics.extend(diagnostics)

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
    <title>Clang-Tidy Report - {file_name}</title>
    <style>
        body {{ font-family: Arial, sans-serif; margin: 20px; background-color: #f5f5f5; }}
        .header {{ background-color: #2c3e50; color: white; padding: 20px; border-radius: 5px; margin-bottom: 20px; }}
        .summary {{ background-color: white; padding: 15px; border-radius: 5px; margin-bottom: 20px; }}
        .diagnostic {{ background-color: white; padding: 10px; border-radius: 3px; margin-bottom: 8px; border-left: 4px solid #e74c3c; }}
        .diagnostic.warning {{ border-left-color: #f39c12; }}
        .diagnostic.info {{ border-left-color: #3498db; }}
        .diagnostic.note {{ border-left-color: #95a5a6; }}
        .diagnostic.error {{ border-left-color: #e74c3c; }}
        .file-path {{ font-weight: bold; color: #2c3e50; font-size: 1.1em; margin-bottom: 10px; }}
        .line-info {{ color: #7f8c8d; font-size: 0.9em; }}
        .severity {{ font-weight: bold; }}
        .severity.error {{ color: #e74c3c; }}
        .severity.warning {{ color: #f39c12; }}
        .severity.info {{ color: #3498db; }}
        .severity.note {{ color: #95a5a6; }}
        .message {{ margin-top: 8px; }}
        .timestamp {{ color: #7f8c8d; font-size: 0.9em; }}
        .check-name {{ color: #34495e; font-size: 0.9em; }}
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
        <h1>Clang-Tidy Analysis Report</h1>
        <p>File: {display_path}</p>
        <p class="timestamp">Generated: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}</p>
    </div>

    <div class="summary">
        <h2>Summary</h2>
        <p>Total issues found: {len(file_diagnostics)}</p>
        <p>Third-party warnings have been filtered out.</p>
    </div>
"""

        if file_diagnostics:
            html_content += '<h2>Issues Found</h2>'

            for diagnostic in file_diagnostics:
                severity = diagnostic.get('severity', 'unknown')
                message = diagnostic.get('message', 'No message')
                check_name = diagnostic.get('checkName', 'unknown')

                # Get line and column information
                line = diagnostic.get('line', 'Unknown')
                column = diagnostic.get('column', 'Unknown')

                html_content += f"""
    <div class="diagnostic {severity}">
        <div class="line-info">Line {line}, Column {column}</div>
        <div class="severity {severity}">[{severity.upper()}] {check_name}</div>
        <div class="message">{message}</div>
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
        html_file = output_dir / f'{safe_filename}-clang-tidy-report.html'
        with open(html_file, 'w', encoding='utf-8') as f:
            f.write(html_content)

        return len(file_diagnostics)

    except (json.JSONDecodeError, FileNotFoundError) as e:
        print(f"Error parsing JSON report: {e}")
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


def parse_clang_tidy_output(output: str) -> list:
    """Parse clang-tidy output and convert to JSON format."""
    json_data = []
    current_file = None
    current_diagnostics = []

    lines = output.strip().split('\n')

    for line in lines:
        line = line.strip()
        if not line:
            continue

        # Check if this is a file path (starts with /)
        if line.startswith('/') and ('error:' in line or 'warning:' in line or 'note:' in line):
            # Save previous file data
            if current_file and current_diagnostics:
                json_data.append({
                    'file': current_file,
                    'diagnostics': current_diagnostics
                })

            # Start new file
            parts = line.split(':', 2)  # Split into file, line, rest
            if len(parts) >= 3:
                current_file = parts[0]
                line_num = parts[1]
                rest = parts[2]

                # Parse severity and message
                if 'error:' in rest:
                    severity = 'error'
                    message = rest.split('error:', 1)[1].strip()
                elif 'warning:' in rest:
                    severity = 'warning'
                    message = rest.split('warning:', 1)[1].strip()
                elif 'note:' in rest:
                    severity = 'note'
                    message = rest.split('note:', 1)[1].strip()
                else:
                    severity = 'info'
                    message = rest.strip()

                current_diagnostics = [{
                    'severity': severity,
                    'line': int(line_num) if line_num.isdigit() else 0,
                    'column': 0,
                    'message': message,
                    'checkName': 'clang-tidy'
                }]
            else:
                current_file = line
                current_diagnostics = []
        elif current_file and ('error:' in line or 'warning:' in line or 'note:' in line):
            # Additional diagnostic for current file
            if 'error:' in line:
                severity = 'error'
                message = line.split('error:', 1)[1].strip()
            elif 'warning:' in line:
                severity = 'warning'
                message = line.split('warning:', 1)[1].strip()
            elif 'note:' in line:
                severity = 'note'
                message = line.split('note:', 1)[1].strip()
            else:
                severity = 'info'
                message = line.strip()

            current_diagnostics.append({
                'severity': severity,
                'line': 0,
                'column': 0,
                'message': message,
                'checkName': 'clang-tidy'
            })

    # Save last file data
    if current_file and current_diagnostics:
        json_data.append({
            'file': current_file,
            'diagnostics': current_diagnostics
        })

    return json_data


def run_clang_tidy(files: list, folder_name: str, verbose: bool = False, save_raw_json: bool = False) -> bool:
    """Run clang-tidy on the specified files."""
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
    report_dir = build_dir / 'clang-tidy-reports' / folder_name
    report_dir.mkdir(parents=True, exist_ok=True)

    # Define report files
    json_report = report_dir / 'clang-tidy-report.json'

    # Build clang-tidy command
    cmd = [
        'clang-tidy',
        '--checks=*',  # Enable all checks
        '--header-filter=.*',  # Check headers too
        '--quiet',  # Suppress warnings about missing compilation database
        '--format-style=file',  # Use .clang-format for formatting
        f'--export-fixes={report_dir}/clang-tidy-fixes.yaml'  # Export fixes
    ]

    # Add compilation database if available
    compile_commands = project_root / 'build' / 'compile_commands.json'
    if compile_commands.exists():
        cmd.extend(['-p', str(project_root / 'build')])
        print("Using compilation database from build/compile_commands.json")
    else:
        print("Warning: No compilation database found. Some checks may not work properly.")
        print("Consider building the project first to generate compile_commands.json")

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

        # Print stderr (clang-tidy errors/warnings)
        if result.stderr:
            print("Clang-Tidy Errors/Warnings:")
            print(result.stderr)

        # Parse stdout (clang-tidy output) and convert to JSON format
        if result.stdout:
            # Parse clang-tidy output and convert to JSON format
            json_data = parse_clang_tidy_output(result.stdout)

            with open(json_report, 'w', encoding='utf-8') as f:
                json.dump(json_data, f, indent=2)

            print(f"\nGenerating individual file reports from {json_report}...")

            file_reports = []
            total_issues = 0

            for file_path in files:
                issue_count = generate_file_report(json_report, report_dir, file_path, folder_name)
                total_issues += issue_count

                # Create report entry for index
                safe_filename = Path(file_path).name.replace('.', '_').replace('/', '_')
                report_path = f'{safe_filename}-clang-tidy-report.html'

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
            if not save_raw_json:
                json_report.unlink()
                print("Raw JSON report cleaned up (use --save-raw-json to keep it)")
            else:
                print(f"Raw JSON report saved to {json_report}")
        else:
            print("No output generated. Check clang-tidy output above.")
            return False

        # Return success if no errors found
        return result.returncode == 0

    except FileNotFoundError:
        print("✗ clang-tidy not found. Please install clang-tidy:")
        print("  sudo apt install clang-tidy")
        return False


def main():
    """Main function."""
    parser = argparse.ArgumentParser(
        description="Run clang-tidy static analysis on C/C++ source files",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python scripts/clang_tidy_analyzer.py src/vnelogging
  python scripts/clang_tidy_analyzer.py src/vnelogging --verbose
  python scripts/clang_tidy_analyzer.py --file src/vertexnova/logging/logging.cpp
  python scripts/clang_tidy_analyzer.py tests
  python scripts/clang_tidy_analyzer.py src/vnelogging --save-raw-json
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
        '--save-raw-json',
        action='store_true',
        help='Save the raw JSON report (normally deleted after processing)'
    )

    args = parser.parse_args()

    # Get the project root directory
    script_dir = Path(__file__).parent
    project_root = script_dir.parent

    print("VneLogging Clang-Tidy Analyzer")
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

    # Run clang-tidy
    success = run_clang_tidy(source_files, folder_name, args.verbose, args.save_raw_json)

    if success:
        print("\n✓ Clang-Tidy analysis completed successfully!")
        print(f"HTML reports saved to build/clang-tidy-reports/{folder_name}/")
    else:
        print("\n✗ Clang-Tidy analysis found issues!")
        print("Please review the output above and fix any issues.")
        sys.exit(1)


if __name__ == "__main__":
    main()
