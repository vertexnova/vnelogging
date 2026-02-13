# vnelogging

**A modern, high-performance C++ logging library** for VertexNova and your C++ projects.

---

## Quick Links

| [Quick Start](Quick-Start) | [Installation](Installation) | [User Guide](User-Guide) | [API Reference](API-Reference) |
|---------------------------|-----------------------------|--------------------------|--------------------------------|

---

## Features

- **Dual modes**: Synchronous (debugging) and asynchronous (production)
- **Multiple sinks**: Console (with colors) and file output
- **Thread-safe**: Safe concurrent logging
- **Stream API**: `VNE_LOG_INFO << "message" << value;`
- **Customizable patterns**: Timestamp, level, thread ID, file, line
- **Multiple loggers**: Separate loggers for subsystems (physics, render, etc.)
- **Cross-platform**: Linux, macOS, Windows, iOS, Android, Web

---

## 30-Second Example

```cpp
#include <vertexnova/logging/logging.h>

namespace { CREATE_VNE_LOGGER_CATEGORY("app") }

int main() {
    vne::log::LoggerConfig config;
    config.sink = vne::log::LogSinkType::eBoth;
    config.file_path = "app.log";
    config.log_level = vne::log::LogLevel::eDebug;
    vne::log::Logging::configureLogger(config);

    VNE_LOG_INFO << "Started";
    VNE_LOG_DEBUG << "Count: " << 42;
    VNE_LOG_ERROR << "Failed";

    vne::log::Logging::shutdown();
    return 0;
}
```

---

## Learn More

- **[VertexNova Tutorial: Adding Logging](https://learnvertexnova.com/docs/docs/learn/adding-logging/)** — Step-by-step guide for adding vnelogging to your project
- **[GitHub Repository](https://github.com/vertexnova/vnelogging)** — Source code, issues, examples
