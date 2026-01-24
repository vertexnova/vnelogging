# VneLogging Scripts

This directory contains utility scripts for building, testing, and maintaining the VneLogging library.

## Build Scripts

### Linux Build (`build_linux.sh`)

Builds VneLogging on Linux using GCC or Clang.

```bash
# Basic build
./scripts/build_linux.sh -t Debug -a configure_and_build

# Select compiler and parallel jobs
./scripts/build_linux.sh -c clang -j 20 -t Release

# Interactive mode
./scripts/build_linux.sh -interactive

# Clean build
./scripts/build_linux.sh -clean -t Debug
```

**Options:**
- `-t, --build-type <type>` - Build type: Debug, Release, RelWithDebInfo, MinSizeRel
- `-c, --compiler <compiler>` - Compiler: gcc or clang
- `-a, --action <action>` - Action: configure, build, configure_and_build, test
- `-j, --jobs <N>` - Number of parallel jobs
- `-clean` - Clean build directory before building
- `-interactive` - Interactive mode for configuration
- `-h, --help` - Show help message

### macOS Build (`build_macos.sh`)

Builds VneLogging on macOS using Clang or Xcode.

```bash
# Basic build
./scripts/build_macos.sh -t Debug -a configure_and_build

# Generate Xcode project
./scripts/build_macos.sh -xcode -t Release

# Interactive mode
./scripts/build_macos.sh -interactive
```

**Options:**
- `-t, --build-type <type>` - Build type
- `-xcode` - Generate Xcode project instead of building
- `-a, --action <action>` - Action: configure, build, configure_and_build, test
- `-j, --jobs <N>` - Number of parallel jobs
- `-clean` - Clean build directory
- `-interactive` - Interactive mode
- `-h, --help` - Show help

### Windows Build

#### Python Script (`build_windows.py`) - Recommended

Native Windows build script with better error handling.

```bash
# Basic build
python scripts/build_windows.py -t Debug -a configure_and_build

# Select Visual Studio version
python scripts/build_windows.py --vs-version 2022 -t Release

# Interactive mode
python scripts/build_windows.py --interactive
```

**Options:**
- `-t, --build-type <type>` - Build type
- `--vs-version <version>` - Visual Studio version (2019, 2022)
- `-a, --action <action>` - Action: configure, build, configure_and_build, test
- `-j, --jobs <N>` - Number of parallel jobs
- `--clean` - Clean build directory
- `--interactive` - Interactive mode
- `-h, --help` - Show help

#### Bash Script (`build_windows.sh`)

For Git Bash or WSL users.

```bash
./scripts/build_windows.sh -t Debug -a configure_and_build
./scripts/build_windows.sh -j 8 -t Release
```

### Web Build (`build_web.sh`)

Builds VneLogging for Web using Emscripten.

```bash
# Basic build
./scripts/build_web.sh Debug

# Force rebuild with parallel jobs
./scripts/build_web.sh -f -j4 Release
```

**Options:**
- `<build_type>` - Build type (Debug, Release, etc.)
- `-f, --force` - Force rebuild
- `-j, --jobs <N>` - Number of parallel jobs
- `-h, --help` - Show help

### iOS Build (`build_ios.sh`)

Builds VneLogging for iOS (Simulator or Device).

```bash
# Basic build
./scripts/build_ios.sh -t Debug -a configure_and_build

# Build for device
./scripts/build_ios.sh -device -deployment-target 15.0

# Generate Xcode project only
./scripts/build_ios.sh -xcode-only -t Release
```

**Options:**
- `-t, --build-type <type>` - Build type
- `-device` - Build for device (default: simulator)
- `-deployment-target <version>` - iOS deployment target version
- `-xcode-only` - Generate Xcode project without building
- `-a, --action <action>` - Action: configure, build, configure_and_build, test
- `-j, --jobs <N>` - Number of parallel jobs
- `-clean` - Clean build directory
- `-h, --help` - Show help

## Code Quality Scripts

### Code Formatting (`clang_formatter.py`)

Formats C++ source files using clang-format.

```bash
# Format entire directory
python scripts/clang_formatter.py src/vnelogging

# Format single file
python scripts/clang_formatter.py --file src/vertexnova/logging/logging.cpp

# Preview changes without modifying files
python scripts/clang_formatter.py src/vnelogging --dry-run

# Verbose output
python scripts/clang_formatter.py src/vnelogging --verbose
```

**Options:**
- `<folder>` - Directory to format
- `--file <file>` - Single file to format
- `--dry-run` - Preview changes without modifying files
- `--verbose` - Show verbose output
- `-h, --help` - Show help

### Static Analysis

#### Combined Analyzer (`static_analyzer.py`)

Runs both cppcheck and clang-tidy analysis.

```bash
# Run both analyzers
python scripts/static_analyzer.py src/vnelogging

# Run only cppcheck
python scripts/static_analyzer.py src/vnelogging --cppcheck-only

# Run only clang-tidy
python scripts/static_analyzer.py src/vnelogging --clang-tidy-only

# Analyze single file
python scripts/static_analyzer.py --file src/vertexnova/logging/logging.cpp
```

**Options:**
- `<folder>` - Directory to analyze
- `--file <file>` - Single file to analyze
- `--cppcheck-only` - Run only cppcheck
- `--clang-tidy-only` - Run only clang-tidy
- `--verbose` - Show verbose output
- `--save-raw-xml` - Keep raw XML report from cppcheck
- `--save-raw-json` - Keep raw JSON report from clang-tidy
- `-h, --help` - Show help

**Reports:** Generated HTML reports are saved to `build/cppcheck-reports/` and `build/clang-tidy-reports/`

#### Clang-Tidy Analyzer (`clang_tidy_analyzer.py`)

Runs clang-tidy static analysis.

```bash
python scripts/clang_tidy_analyzer.py src/vnelogging
python scripts/clang_tidy_analyzer.py --file src/vertexnova/logging/logging.cpp --verbose
```

#### Cppcheck Analyzer (`cppcheck_analyzer.py`)

Runs cppcheck static analysis.

```bash
python scripts/cppcheck_analyzer.py src/vnelogging
python scripts/cppcheck_analyzer.py --file src/vertexnova/logging/logging.cpp --verbose
```

## Testing Scripts

### Test Filter (`run_google_test_filter.sh`)

Run Google Test with filters to execute specific test cases.

```bash
# Run all tests
./scripts/run_google_test_filter.sh build/Debug/build-linux-gcc-12.0.0/bin/TestVneLogging

# Run tests matching a pattern
./scripts/run_google_test_filter.sh build/Debug/build-linux-gcc-12.0.0/bin/TestVneLogging "LogStream.*"

# List all available tests
./scripts/run_google_test_filter.sh build/Debug/build-linux-gcc-12.0.0/bin/TestVneLogging --list

# Verbose output
./scripts/run_google_test_filter.sh build/Debug/build-linux-gcc-12.0.0/bin/TestVneLogging "LogStream.*" --verbose
```

**Options:**
- `<test_binary>` - Path to test executable (required)
- `<filter>` - Test filter pattern (optional)
- `-l, --list` - List all available tests
- `-v, --verbose` - Enable verbose output
- `-h, --help` - Show help

**Filter Examples:**
- `"LogStream.*"` - Run all tests starting with "LogStream"
- `"*FileSink*"` - Run all tests containing "FileSink"
- `"Test1:Test2"` - Run Test1 and Test2
- `"-*Slow*"` - Run all tests except those containing "Slow"

## Coverage Scripts

### Check Coverage (`check_coverage.sh`)

Verifies that code coverage is enabled and working correctly.

```bash
# Check coverage status
./scripts/check_coverage.sh -d build/Debug/build-macos-clang-17.0.0
./scripts/check_coverage.sh -d build/Debug/build-linux-gcc-12.0.0
```

**Options:**
- `-d, --build-dir <dir>` - Build directory (required)
- `-h, --help` - Show help

**Note:** Coverage must be enabled during CMake configuration with `-DENABLE_COVERAGE=ON`

### Generate Coverage Report (`generate_coverage.sh`)

Generates HTML code coverage reports for macOS (LLVM) or Linux (GCC/lcov).

```bash
# Generate coverage report
./scripts/generate_coverage.sh -d build/Debug/build-macos-clang-17.0.0

# Filter to only src/vnelogging (exclude tests and third-party)
./scripts/generate_coverage.sh -d build/Debug/build-linux-gcc-12.0.0 -f

# Use existing coverage data (skip test execution)
./scripts/generate_coverage.sh -d build/Debug/build-macos-clang-17.0.0 --no-tests

# Specify test executable
./scripts/generate_coverage.sh -d build/Debug/build-linux-gcc-12.0.0 -e ./bin/TestVneLogging
```

**Options:**
- `-d, --build-dir <dir>` - Build directory (required)
- `-t, --build-type <type>` - Build type (default: Debug)
- `-e, --executable <exe>` - Test executable (default: auto-detect)
- `-f, --filter-src-only` - Filter to only src/vnelogging
- `-n, --no-open` - Don't open report in browser
- `--no-tests` - Don't run tests (use existing coverage data)
- `-h, --help` - Show help

**Report Location:** `build/<build_dir>/coverage_report/index.html`

## Documentation Scripts

### Generate Documentation (`generate-docs.sh`)

Generates API documentation using Doxygen and validates documentation.

```bash
# Generate all documentation
./scripts/generate-docs.sh

# Generate only API documentation
./scripts/generate-docs.sh --api-only

# Validate existing documentation (links, code examples)
./scripts/generate-docs.sh --validate
```

**Options:**
- `--api-only` - Generate only API documentation
- `--validate` - Only validate existing documentation
- `-h, --help` - Show help

**Requirements:**
- Doxygen (for API documentation)
- markdown-link-check (for link validation)

**Output:** Documentation is generated in `docs/api/generated/`

## Prerequisites

### Build Tools
- CMake 3.16 or higher
- C++20 compatible compiler (GCC 10+, Clang 12+, MSVC 2019+)

### Code Quality Tools (Optional)
- **clang-format** - Code formatting
- **clang-tidy** - Static analysis
- **cppcheck** - Additional static analysis

Installation:
```bash
# Ubuntu/Debian
sudo apt install clang-format clang-tidy cppcheck

# macOS
brew install llvm cppcheck
```

### Coverage Tools (Optional)
- **lcov** (Linux) - Code coverage for GCC
- **llvm-cov** (macOS) - Code coverage for Clang (included with Xcode)

Installation:
```bash
# Ubuntu/Debian
sudo apt install lcov

# macOS (included with Xcode Command Line Tools)
xcode-select --install
```

### Documentation Tools (Optional)
- **doxygen** - API documentation generation
- **markdown-link-check** - Link validation

Installation:
```bash
# Ubuntu/Debian
sudo apt install doxygen
npm install -g markdown-link-check

# macOS
brew install doxygen
npm install -g markdown-link-check
```

## Common Workflows

### Building and Testing
```bash
# Build Debug version with tests
./scripts/build_linux.sh -t Debug -a configure_and_build

# Run tests
cd build/Debug/build-linux-gcc-12.0.0
ctest --output-on-failure
```

### Code Quality Check
```bash
# Format code
python scripts/clang_formatter.py src/vnelogging

# Run static analysis
python scripts/static_analyzer.py src/vnelogging
```

### Coverage Analysis
```bash
# Build with coverage enabled
./scripts/build_linux.sh -t Debug -a configure
cd build/Debug/build-linux-gcc-12.0.0
cmake . -DENABLE_COVERAGE=ON
cmake --build . -j8

# Generate coverage report
./scripts/generate_coverage.sh -d build/Debug/build-linux-gcc-12.0.0 -f
```

### Documentation
```bash
# Generate API documentation
./scripts/generate-docs.sh --api-only
```
