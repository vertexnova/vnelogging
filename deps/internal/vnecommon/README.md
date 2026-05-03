<p align="center">
  <img src="icons/vertexnova_logo_medallion_with_text.svg" alt="VertexNova Common" width="320"/>
</p>

<p align="center">
  <strong>Common utilities for VertexNova projects</strong>
</p>

<p align="center">
  <a href="https://github.com/vertexnova/vnecommon/actions/workflows/ci.yml">
    <img src="https://github.com/vertexnova/vnecommon/actions/workflows/ci.yml/badge.svg?branch=main" alt="CI"/>
  </a>
  <img src="https://img.shields.io/badge/C%2B%2B-20-blue.svg" alt="C++ Standard"/>
  <img src="https://img.shields.io/badge/header--only-yes-brightgreen.svg" alt="Header Only"/>
  <img src="https://img.shields.io/badge/license-Apache%202.0-green.svg" alt="License"/>
</p>

---

## About

Header-only library providing platform detection, macros, and base classes (e.g. NonCopyable) for VertexNova projects.

## Features

- **Platform Detection** - Macros for Windows, macOS, iOS, visionOS, Android, Linux, Web
- **Compiler Detection** - MSVC, GCC, Clang identification
- **Architecture Detection** - x64, x86, ARM64, ARM
- **Assertions** - Debug-only assertions with messages
- **Utility Macros** - Force inline, branch hints, unused variable marking
- **NonCopyable / NonMovable / NonCopyableNonMovable** - Base classes to disable copy and/or move; inherit privately

## Headers

| Header | Description |
|--------|-------------|
| `vertexnova/common/common.h` | Main include (includes all headers) |
| `vertexnova/common/platform.h` | Platform, compiler, architecture detection |
| `vertexnova/common/macros.h` | Assertions and utility macros |
| `vertexnova/common/noncopyable.h` | NonCopyable, NonMovable, NonCopyableNonMovable base classes |

## Usage

Add as a submodule:

```bash
git submodule add git@github.com:vertexnova/vnecommon.git libs/vnecommon
```

In your CMakeLists.txt:

```cmake
add_subdirectory(libs/vnecommon)
target_link_libraries(your_target PRIVATE vne::common)
```

In your C++ code:

```cpp
#include "vertexnova/common/common.h"

void example() {
    VNE_ASSERT_MSG(ptr != nullptr, "Pointer must not be null");
    VNE_UNUSED(some_variable);

#if defined(VNE_PLATFORM_MACOS)
    // macOS-specific code
#endif
}

// Disable copy/move via base classes (inherit privately)
class UniqueResource : private vertexnova::NonCopyable { /* ... */ };
class Singleton : private vertexnova::NonCopyableNonMovable { /* ... */ };
```

## Building

### macOS / Linux

```bash
# Debug build with examples
./scripts/build_macos.sh

# Release build
./scripts/build_macos.sh --release

# Clean build
./scripts/build_macos.sh --clean
```

### Windows

```powershell
# Debug build with examples
python scripts/build_windows.py

# Release build
python scripts/build_windows.py --release

# Clean build
python scripts/build_windows.py --clean
```

### CMake Direct

```bash
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release -DBUILD_EXAMPLES=ON
cmake --build build --config Release
```

## Requirements

- C++20 or later
- CMake 3.16 or later
- Python 3.8+ (for Windows build script)

## License

Apache License 2.0 — See [LICENSE](LICENSE) for details.

---

<p align="center">
  Part of the <a href="https://github.com/vertexnova">VertexNova</a> project
</p>
