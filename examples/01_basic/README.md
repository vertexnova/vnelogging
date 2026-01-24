# 01 - Basic Logging Example

This example demonstrates the fundamental usage of the VNE Logging library.

## What This Example Shows

1. **Logger Configuration**: Setting up a logger with console and file sinks
2. **Log Levels**: Using all available log levels (TRACE, DEBUG, INFO, WARN, ERROR, FATAL)
3. **Log Patterns**: Customizing output format for console and file
4. **Stream-style Logging**: Using the `<<` operator to log messages with variables

## Building

```bash
cd build
cmake .. -DBUILD_EXAMPLES=ON
cmake --build .
```

## Running

```bash
./bin/01_BasicLogging
```

## Expected Output

```
2026-01-24 02:17:24 [TRACE] [basic.example] This is a trace message - most detailed level
2026-01-24 02:17:24 [DEBUG] [basic.example] This is a debug message - useful for debugging
2026-01-24 02:17:24 [INFO] [basic.example] This is an info message - general information
2026-01-24 02:17:24 [WARN] [basic.example] This is a warning message - something might be wrong
2026-01-24 02:17:24 [ERROR] [basic.example] This is an error message - something went wrong
2026-01-24 02:17:24 [FATAL] [basic.example] This is a fatal message - critical failure
2026-01-24 02:17:24 [INFO] [basic.example] User: Alice, Count: 42
2026-01-24 02:17:24 [DEBUG] [basic.example] Debug mode is enabled
```

A file `basic_example.log` will also be created with the same messages.

## Key Concepts

- `LoggerConfig`: Configuration structure for logger setup
- `LogSinkType::eBoth`: Output to both console and file
- `VNE_LOG_*` macros: Convenient logging at different levels
- `CREATE_VNE_LOGGER_CATEGORY`: Define a category for log messages
