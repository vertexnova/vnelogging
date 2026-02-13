# Quick Start

Get vnelogging running in 3 steps: **Configure → Log → Shutdown**.

## 1. Add to your project

**Git submodule:**
```bash
git submodule add https://github.com/vertexnova/vnelogging.git external/vnelogging
```

**CMakeLists.txt:**
```cmake
add_subdirectory(external/vnelogging)
target_link_libraries(your_target PRIVATE vne::logging)
```

## 2. Configure and log

```cpp
#include <vertexnova/logging/logging.h>

namespace {
CREATE_VNE_LOGGER_CATEGORY("app")
}  // namespace

int main() {
    vne::log::LoggerConfig config;
    config.name = vne::log::kDefaultLoggerName;
    config.sink = vne::log::LogSinkType::eBoth;
    config.file_path = "app.log";
    config.log_level = vne::log::LogLevel::eDebug;

    vne::log::Logging::configureLogger(config);

    VNE_LOG_INFO << "Application started";
    VNE_LOG_DEBUG << "Processing " << count << " items";
    VNE_LOG_ERROR << "Failed to load: " << filename;

    vne::log::Logging::shutdown();
    return 0;
}
```

## 3. Build and run

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/bin/your_app
```

---

**Next:** [Installation](Installation) for more options, or [User Guide](User-Guide) for configuration details.
