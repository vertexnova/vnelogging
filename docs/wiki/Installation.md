# Installation

## Option 1: Git Submodule (Recommended)

```bash
git submodule add https://github.com/vertexnova/vnelogging.git external/vnelogging
```

In your `CMakeLists.txt`:
```cmake
add_subdirectory(external/vnelogging)
target_link_libraries(your_target PRIVATE vne::logging)
```

## Option 2: FetchContent

```cmake
include(FetchContent)
FetchContent_Declare(
    vnelogging
    GIT_REPOSITORY https://github.com/vertexnova/vnelogging.git
    GIT_TAG main
)
FetchContent_MakeAvailable(vnelogging)
target_link_libraries(your_target PRIVATE vne::logging)
```

## Option 3: System Install

```bash
git clone --recursive https://github.com/vertexnova/vnelogging.git
cd vnelogging
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local
cmake --build build
sudo cmake --install build
```

In your `CMakeLists.txt`:
```cmake
find_package(VneLogging REQUIRED)
target_link_libraries(your_target PRIVATE vne::logging)
```

---

## CMake Options

| Option | Default | Description |
|--------|---------|-------------|
| `BUILD_TESTS` | `ON` | Build unit tests |
| `BUILD_EXAMPLES` | `OFF` | Build example applications |
| `ENABLE_COVERAGE` | `OFF` | Enable code coverage (Debug only) |

---

## Requirements

- C++17 or later
- CMake 3.16+
- GCC 9+, Clang 10+, or MSVC 2019+
