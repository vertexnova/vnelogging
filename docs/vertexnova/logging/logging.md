# Logging Module

## Overview

The Logging module provides a comprehensive, flexible, and efficient logging system for C++ applications. It supports multiple log sinks, configurable log levels, customizable formatting, and both synchronous and asynchronous logging operations.

## Architecture

The logging system follows a layered architecture with clear separation of concerns:

- **Logging Interface Layer**: High-level API for application integration
- **Logger Management Layer**: Logger lifecycle and configuration management
- **Sink Layer**: Output destination management (console, file)
- **Formatting Layer**: Message formatting and timestamp handling
- **Queue Layer**: Asynchronous logging support

## Key Components

### Core Classes

#### `Logging_C`

Main interface class providing static methods for logging system setup and configuration.

**Key Methods:**

- `Initialize(logger_name, async)`: Initialize the logging system
- `Shutdown()`: Clean shutdown of the logging system
- `GetLogger(name)`: Retrieve a logger instance
- `AddConsoleSink(logger_name)`: Add console output
- `AddFileSink(logger_name, file)`: Add file output
- `SetLogLevel(logger_name, level)`: Configure log level
- `SetConsolePattern(logger_name, pattern)`: Set console formatting
- `SetFilePattern(logger_name, pattern)`: Set file formatting

#### `LogManager_C`

Singleton class managing logger instances and their lifecycle.

**Responsibilities:**

- Logger registration and retrieval
- Global configuration management
- Logger factory operations

#### `Logger_I` Interface

Abstract base class defining the logger interface.

**Key Methods:**

- `Log(category, level, timestamp_type, message, file, function, line)`: Core logging operation
- `AddLogSink(sink)`: Add output destination
- `SetCurrentLogLevel(level)`: Configure filtering
- `Flush()`: Force output flush
- `Clone(name)`: Create logger copy

### Logger Implementations

#### `SyncLogger_C`

Synchronous logger implementation for immediate output.

**Features:**

- Direct message processing
- Thread-safe operations
- Immediate output to sinks

#### `AsyncLogger_C`

Asynchronous logger implementation for high-performance logging.

**Features:**

- Background message processing
- Queue-based architecture
- Non-blocking operations

### Sink System

#### `LogSink_I` Interface

Abstract interface for log output destinations.

**Methods:**

- `Log(level, message)`: Output a log message
- `Flush()`: Force output flush

#### `ConsoleLogSink_C`

Console output sink with color support.

**Features:**

- Colored output based on log level
- Configurable formatting patterns
- Platform-specific console handling

#### `FileLogSink_C`

File output sink with rotation support.

**Features:**

- File rotation and management
- Configurable file paths
- Automatic directory creation

### Formatting System

#### `LogFormatter_C`

Handles message formatting according to patterns.

**Pattern Variables:**

- `%t`: Timestamp
- `%l`: Log level
- `%n`: Logger name
- `%c`: Category
- `%m`: Message
- `%f`: File name
- `%F`: Function name
- `%L`: Line number

#### `TimeStamp_C`

Timestamp generation and formatting.

**Features:**

- Local and UTC time support
- Configurable format strings
- High-resolution timestamps

#### `TextColor_C`

Console color management for different log levels.

**Color Mapping:**

- TRACE: Gray
- DEBUG: Blue
- INFO: Green
- WARN: Yellow
- ERROR: Red
- FATAL: Bold Red

### Queue System (Async Logging)

#### `LogQueue_C`

Thread-safe message queue for asynchronous logging.

**Features:**

- Lock-free operations
- Configurable queue size
- Overflow handling

#### `LogQueueWorker_C`

Background worker processing queued messages.

**Features:**

- Continuous message processing
- Graceful shutdown
- Error handling

#### `LogDispatcher_C`

Routes messages to appropriate sinks.

**Features:**

- Multi-sink support
- Level-based filtering
- Sink-specific formatting

## Log Levels

The system supports six log levels in ascending order of severity:

```cpp
enum class LogLevel_TP {
    LOG_TRACE = 0,  // Detailed diagnostic information
    LOG_DEBUG = 1,  // Debugging information
    LOG_INFO = 2,   // General information
    LOG_WARN = 3,   // Warning messages
    LOG_ERROR = 4,  // Error conditions
    LOG_FATAL = 5   // Fatal errors
};
```

## Configuration

### Logger Configuration

```cpp
struct LoggerConfig_C {
    std::string name;                    // Logger name
    LogSink_TP sink;                     // Output destinations
    std::string console_pattern;         // Console format pattern
    std::string file_pattern;            // File format pattern
    std::string file_path;               // Log file path
    LogLevel_TP log_level;               // Minimum log level
    LogLevel_TP flush_level;             // Auto-flush level
    bool async;                          // Async mode flag
};
```

### Sink Types

```cpp
enum class LogSink_TP {
    NONE = 0,              // No output
    CONSOLE = 1 << 0,      // Console output
    FILE = 1 << 1,         // File output
    BOTH = CONSOLE | FILE  // Both outputs
};
```

## Usage Examples

### Basic Initialization

```cpp
#include "vertexnova/logging//logging.h"

// Initialize with default settings
VNE::Log::Logging_C::Initialize("myapp");

// Initialize with async logging
VNE::Log::Logging_C::Initialize("myapp", true);
```

### Logger Configuration

```cpp
// Add console output
VNE::Log::Logging_C::AddConsoleSink("myapp");

// Add file output
VNE::Log::Logging_C::AddFileSink("myapp", "app.log");

// Set log level
VNE::Log::Logging_C::SetLogLevel("myapp", VNE::Log::LogLevel_TP::LOG_DEBUG);

// Configure patterns
VNE::Log::Logging_C::SetConsolePattern("myapp", "[%t] %l [%n] %m");
VNE::Log::Logging_C::SetFilePattern("myapp", "%t | %l | %n | %f:%L | %m");
```

### Logging Messages

```cpp
// Get logger instance
auto logger = VNE::Log::Logging_C::GetLogger("myapp");

// Log messages
logger->Log("core.init", VNE::Log::LogLevel_TP::LOG_INFO,
            VNE::Log::TimeStampType_TP::LOCAL,
            "Application started", __FILE__, __FUNCTION__, __LINE__);

logger->Log("core.shutdown", VNE::Log::LogLevel_TP::LOG_WARN,
            VNE::Log::TimeStampType_TP::LOCAL,
            "Application shutting down", __FILE__, __FUNCTION__, __LINE__);
```

### Using Log Streams

```cpp
#include "vertexnova/logging//core/log_stream.h"

// Create log stream
VNE::Log::LogStream_C stream("myapp", "core.rendering", VNE::Log::LogLevel_TP::LOG_DEBUG);

// Stream messages
stream << "Rendering frame " << frameNumber << " at " << timestamp;
```

## Performance Considerations

### Synchronous vs Asynchronous

- **Synchronous**: Immediate output, suitable for debugging and small applications
- **Asynchronous**: Higher performance, suitable for production applications

### Queue Management

- Configure appropriate queue sizes for async logging
- Monitor queue overflow conditions
- Use flush levels to ensure critical messages are output immediately

### File I/O Optimization

- Use buffered file operations
- Implement log rotation to manage file sizes
- Consider compression for archived logs

## Best Practices

### Logger Organization

- Use descriptive logger names (e.g., "physics", "network", "ui")
- Organize categories hierarchically (e.g., "core.init", "graphics.rendering")
- Separate concerns with different loggers

### Message Quality

- Use appropriate log levels
- Include relevant context in messages
- Avoid logging sensitive information
- Use structured logging for complex data

### Configuration Management

- Configure log levels per environment
- Use patterns for consistent formatting
- Implement log rotation policies
- Monitor log file sizes and disk usage

## Integration

### With Other Modules

The logging system integrates with other VertexNova modules:

- **IO Module**: Log file operations and errors
- **Thread Module**: Thread-safe logging operations
- **Core Modules**: Application lifecycle logging

### External Dependencies

- **spdlog**: Optional integration for advanced features
- **Standard Library**: File system and threading support
- **Platform APIs**: Console and file system operations

## Version History

- **1.0.0**: Initial release with core logging functionality
- **1.1.0**: Added asynchronous logging support
- **1.2.0**: Enhanced formatting and sink management
- **1.3.0**: Improved performance and thread safety

## Dependencies

- C++17 or higher
- Standard Library (filesystem, threading, memory)
- Platform-specific console APIs
- Optional: spdlog for advanced features

## Testing

The logging module includes comprehensive tests covering:

- Logger creation and configuration
- Message formatting and output
- Sink operations (console and file)
- Asynchronous logging performance
- Thread safety and concurrency
- Error handling and edge cases

Run tests with:

```bash
# Run all logging tests
ctest -R logging

# Run specific test categories
ctest -R logging_core
ctest -R logging_sinks
ctest -R logging_async
```
