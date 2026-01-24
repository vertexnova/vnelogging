<p align="center">
  <img src="icons/vertexnova_logo_medallion_with_text.svg" alt="VertexNova Logging" width="200"/>
</p>

<p align="center">
  <strong>A modern, high-performance C++ logging library</strong>
</p>

<p align="center">
  <a href="https://github.com/vertexnova/vnelogging/actions/workflows/ci.yml">
    <img src="https://github.com/vertexnova/vnelogging/actions/workflows/ci.yml/badge.svg?branch=main" alt="CI"/>
  </a>
  <a href="https://codecov.io/gh/vertexnova/vnelogging">
    <img src="https://codecov.io/gh/vertexnova/vnelogging/branch/main/graph/badge.svg" alt="Coverage"/>
  </a>
  <img src="https://img.shields.io/badge/C%2B%2B-17%2F20-blue.svg" alt="C++ Standard"/>
  <img src="https://img.shields.io/badge/license-Apache%202.0-green.svg" alt="License"/>
</p>

---

## About

**vnelogging** is a lightweight, thread-safe logging library designed for high-performance C++ applications. It provides both synchronous and asynchronous logging modes, multiple output sinks, and a clean stream-based API.

This library is the **first component** of [VertexNova](https://github.com/vertexnova) — a multi-backend game/graphics engine I'm building from scratch. The logging system needed to be fast, flexible, and production-ready before building the rest of the engine on top of it.

## Features

- **Dual Logging Modes**: Synchronous for debugging, asynchronous for production
- **Multiple Sinks**: Console (with colors) and file output
- **Thread-Safe**: Safe concurrent logging from multiple threads
- **Stream API**: Familiar `<<` operator for composing messages
- **Customizable Patterns**: Configure timestamp, level, thread ID, and more
- **Multiple Loggers**: Create separate loggers for different subsystems
- **Cross-Platform**: Linux, macOS, and Windows support
- **Header-Only Option**: Easy integration into existing projects

## Quick Start

### Building

```bash
git clone --recursive https://github.com/vertexnova/vnelogging.git
cd vnelogging
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

### Basic Usage

```cpp
#include <vertexnova/logging/logging.h>

namespace {
CREATE_VNE_LOGGER_CATEGORY("app")
}  // namespace

int main() {
    // Configure logger
    vne::log::LoggerConfig config;
    config.name = vne::log::kDefaultLoggerName;
    config.sink = vne::log::LogSinkType::eBoth;
    config.file_path = "app.log";
    config.log_level = vne::log::LogLevel::eDebug;
    
    vne::log::Logging::configureLogger(config);
    
    // Log messages
    VNE_LOG_INFO << "Application started";
    VNE_LOG_DEBUG << "Processing " << item_count << " items";
    VNE_LOG_ERROR << "Failed to load: " << filename;
    
    vne::log::Logging::shutdown();
    return 0;
}
```

## Examples

| Example | Description |
|---------|-------------|
| [01_hello_logging](examples/01_hello_logging) | Getting started with basic logging |
| [02_subsystem_logging](examples/02_subsystem_logging) | Multiple loggers for different subsystems |
| [03_spdlog_integration](examples/03_spdlog_integration) | Using vnelogging alongside spdlog |
| [04_benchmark](examples/04_benchmark) | Performance comparison: sync vs async |
| [05_multithreaded](examples/05_multithreaded) | Thread-safe logging from concurrent threads |

Build examples with:

```bash
cmake -B build -DBUILD_EXAMPLES=ON
cmake --build build
./build/bin/01_HelloLogging
```

## Performance

Benchmarked on Apple M3 Pro (Release build):

| Mode | Throughput | Latency |
|------|------------|---------|
| Synchronous | ~500K logs/sec | ~2 μs/log |
| Asynchronous | ~1.2M logs/sec | ~0.8 μs/log |

## Documentation

- [API Documentation](docs/README.md) — Generated with Doxygen
- [Architecture](docs/vertexnova/logging/logging.md) — Design and components
- [Coding Guidelines](CODING_GUIDELINES.md) — Project conventions

## Requirements

- C++17 or later
- CMake 3.16+
- Compiler: GCC 9+, Clang 10+, MSVC 2019+

## License

Apache License 2.0 — See [LICENSE](LICENSE) for details.

---

<p align="center">
  Part of the <a href="https://github.com/vertexnova">VertexNova</a> project
</p>
