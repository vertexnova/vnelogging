# User Guide

## Basic Usage

### Logger category

Create a category once per file (usually in an anonymous namespace):

```cpp
namespace {
CREATE_VNE_LOGGER_CATEGORY("myapp")
}  // namespace
```

### Log macros

```cpp
VNE_LOG_TRACE << "Most detailed";
VNE_LOG_DEBUG << "Debug info";
VNE_LOG_INFO  << "Normal operation";
VNE_LOG_WARN  << "Something unusual";
VNE_LOG_ERROR << "Something failed";
VNE_LOG_FATAL << "Cannot continue";
```

### Stream API

Compose messages with `<<`:

```cpp
VNE_LOG_INFO << "User: " << name << ", Count: " << count;
```

---

## Configuration

### LoggerConfig

```cpp
vne::log::LoggerConfig config;
config.name = vne::log::kDefaultLoggerName;   // "vertexnova"
config.sink = vne::log::LogSinkType::eBoth;   // Console + File
config.file_path = "logs/app.log";
config.log_level = vne::log::LogLevel::eDebug;
config.async = false;  // true for high-throughput

vne::log::Logging::configureLogger(config);
```

| Field | Description |
|-------|-------------|
| `name` | Logger identifier |
| `sink` | `eConsole`, `eFile`, or `eBoth` |
| `file_path` | Log file path |
| `log_level` | Minimum level to output |
| `flush_level` | Auto-flush at this level (default: eError) |
| `async` | Use async mode for non-blocking logging |

### Shutdown

Always call before exit:

```cpp
vne::log::Logging::shutdown();
```

---

## Format Patterns

| Pattern | Output |
|---------|--------|
| `%x` | Timestamp |
| `%l` | Log level (INFO, DEBUG, etc.) |
| `%n` | Logger name |
| `%c` | Category |
| `%t` | Thread ID |
| `%v` | Message |
| `%$` | Source file |
| `%!` | Function name |
| `%#` | Line number |

Example:
```cpp
config.console_pattern = "%x [%l] %v";
config.file_pattern = "%x [%l] [%c] [%t] %$ %!:%# %v";
```

---

## Advanced

### Named loggers

```cpp
VNE_LOG_INFO_L("physics") << "Simulation step";
VNE_LOG_DEBUG_L("render") << "Frame drawn";
```

### Multiple loggers

Configure separate loggers for subsystems:

```cpp
vne::log::LoggerConfig physics_config;
physics_config.name = "physics";
physics_config.file_path = "physics.log";
vne::log::Logging::configureLogger(physics_config);

vne::log::LoggerConfig render_config;
render_config.name = "render";
render_config.file_path = "render.log";
render_config.async = true;
vne::log::Logging::configureLogger(render_config);
```

### Async logging

For high-throughput, enable async:

```cpp
config.async = true;
```

### Disable console colors

If colors show as escape codes (e.g. in Xcode debugger):

```cpp
vne::log::Logging::setColorEnabled(false);
```

Or set the `NO_COLOR` environment variable.
